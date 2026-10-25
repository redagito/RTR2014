#include <GLFW/glfw3.h>
#include <json/json.h>

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <memory>
#include <string>

#include "demo/SceneEditor.h"
#include "graphics/ISceneQuery.h"
#include "graphics/camera/FirstPersonCamera.h"
#include "graphics/scene/Scene.h"
#include "input/IInputListener.h"
#include "input/IInputProvider.h"
#include "resource/IResourceManager.h"
#include "resource/OrientedBoundingBox.h"
#include "resource/ResourceManager.h"

namespace
{
class FakeInputProvider : public IInputProvider
{
   public:
    void addInputListener(IInputListener* listener) override {}
    void removeInputListener(IInputListener* listener) override {}
    bool isKeyPressed(int keyCode) override
    {
        const auto entry = m_pressed.find(keyCode);
        return entry != m_pressed.end() && entry->second;
    }

    void press(int keyCode) { m_pressed[keyCode] = true; }
    void release(int keyCode) { m_pressed[keyCode] = false; }

   private:
    std::map<int, bool> m_pressed;
};

class TempDirectory
{
   public:
    TempDirectory()
    {
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path =
            std::filesystem::temp_directory_path() / ("rtr2014_editor_" + std::to_string(unique));
        std::filesystem::create_directories(m_path);
    }

    ~TempDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const { return m_path; }

   private:
    std::filesystem::path m_path;
};

Json::Value readJson(const std::filesystem::path& file)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream input(file);
    REQUIRE(reader.parse(input, root));
    return root;
}
}  // namespace

TEST_CASE("SceneEditor spawns, moves and removes a model", "[demo]")
{
    TempDirectory directory;
    const std::filesystem::path meshDirectory = directory.path() / "mesh";
    const std::filesystem::path materialDirectory = directory.path() / "material";
    std::filesystem::create_directories(meshDirectory);
    std::filesystem::create_directories(materialDirectory);
    std::filesystem::copy_file(std::filesystem::path(RTR_PROJECT_DATA_DIR) / "mesh" / "cube.obj",
                               meshDirectory / "cube.obj");

    {
        std::ofstream material(materialDirectory / "simple.ini");
        material << "[custom]\nkey=value\n";
    }

    const std::filesystem::path sceneFile = directory.path() / "scene.json";
    {
        std::ofstream scene(sceneFile);
        scene << "{\"music\": \"keep.mp3\", \"scene_objects\": []}";
    }

    ResourceManager resourceManager;
    Scene scene;
    FirstPersonCamera camera(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 45.f, 1.33f, 0.01f, 100.f);
    FakeInputProvider input;

    SceneEditor editor(scene, camera, input, resourceManager, sceneFile.string(),
                       meshDirectory.string(), materialDirectory.string());

    REQUIRE_FALSE(editor.isActive());
    input.press(GLFW_KEY_TAB);
    editor.update(0.016f);
    input.release(GLFW_KEY_TAB);
    REQUIRE(editor.isActive());

    input.press(GLFW_KEY_ENTER);
    editor.update(0.016f);
    input.release(GLFW_KEY_ENTER);

    ResourceId mesh;
    ResourceId material;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool castsShadow = false;
    REQUIRE(scene.getObject(0, mesh, material, position, rotation, scale, castsShadow));
    CHECK(castsShadow);
    CHECK(position.z < -2.9f);
    CHECK(scale == glm::vec3(1.f));

    input.press(GLFW_KEY_L);
    editor.update(0.016f);
    input.release(GLFW_KEY_L);

    REQUIRE(scene.getObject(0, mesh, material, position, rotation, scale, castsShadow));
    CHECK(position.x > 0.f);

    // V rotates around the default Y axis and F10 scales the model up.
    input.press(GLFW_KEY_V);
    editor.update(0.016f);
    input.release(GLFW_KEY_V);
    input.press(GLFW_KEY_F10);
    editor.update(0.016f);
    input.release(GLFW_KEY_F10);

    REQUIRE(scene.getObject(0, mesh, material, position, rotation, scale, castsShadow));
    CHECK(rotation.y > 0.f);
    CHECK(scale.x > 1.f);

    REQUIRE(editor.saveScene());
    const Json::Value saved = readJson(sceneFile);
    CHECK(saved["music"].asString() == "keep.mp3");
    REQUIRE(saved["scene_objects"].size() == 1);
    CHECK(saved["scene_objects"][0]["mesh"].asString().find("cube.obj") != std::string::npos);
    CHECK(saved["scene_objects"][0]["rotation"][1].asFloat() > 0.f);
    CHECK(saved["scene_objects"][0]["scale"][0].asFloat() > 1.f);

    input.press(GLFW_KEY_DELETE);
    editor.update(0.016f);
    input.release(GLFW_KEY_DELETE);

    CHECK_FALSE(scene.getObject(0, mesh, material, position, rotation, scale, castsShadow));

    // Saving again drops the removed object from the file.
    REQUIRE(editor.saveScene());
    CHECK(readJson(sceneFile)["scene_objects"].empty());
}

TEST_CASE("A removed scene object is no longer returned by a scene query", "[scene]")
{
    Scene scene;
    const OrientedBoundingBox bounds(glm::vec3(0.f), glm::mat3(1.f), glm::vec3(1.f));

    const SceneObjectId first = scene.createObject(-1, -1, glm::vec3(0.f, 0.f, -10.f),
                                                   glm::vec3(0.f), glm::vec3(1.f), bounds);
    const SceneObjectId second = scene.createObject(-1, -1, glm::vec3(1.f, 0.f, -10.f),
                                                    glm::vec3(0.f), glm::vec3(1.f), bounds);

    scene.removeObject(first);

    ResourceId mesh;
    ResourceId material;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool castsShadow = true;
    CHECK_FALSE(scene.getObject(first, mesh, material, position, rotation, scale, castsShadow));
    CHECK(scene.getObject(second, mesh, material, position, rotation, scale, castsShadow));

    const glm::mat4 view =
        glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    const glm::mat4 projection = glm::perspective(glm::radians(60.f), 1.33f, 0.1f, 100.f);
    const std::unique_ptr<ISceneQuery> query = scene.createQuery(projection * view);

    REQUIRE(query->hasNextObject());
    CHECK(query->getNextObject() == second);
    CHECK_FALSE(query->hasNextObject());
}

TEST_CASE("SceneEditor lists every control in the debug rows", "[demo]")
{
    TempDirectory directory;
    const std::filesystem::path meshDirectory = directory.path() / "mesh";
    const std::filesystem::path materialDirectory = directory.path() / "material";
    std::filesystem::create_directories(meshDirectory);
    std::filesystem::create_directories(materialDirectory);

    const std::filesystem::path sceneFile = directory.path() / "scene.json";
    {
        std::ofstream scene(sceneFile);
        scene << "{\"scene_objects\": []}";
    }

    ResourceManager resourceManager;
    Scene scene;
    FirstPersonCamera camera(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 45.f, 1.33f, 0.01f, 100.f);
    FakeInputProvider input;

    SceneEditor editor(scene, camera, input, resourceManager, sceneFile.string(),
                       meshDirectory.string(), materialDirectory.string());

    std::vector<DebugInfo::Value> rows;
    editor.appendDebugInfo(rows);

    const auto hasRow = [&rows](const std::string& key, const std::string& setting)
    {
        return std::any_of(rows.begin(), rows.end(), [&key, &setting](const DebugInfo::Value& row)
                           { return row.key == key && row.setting == setting; });
    };

    CHECK(hasRow("Tab", "Close editor"));
    CHECK(hasRow("Enter", "Spawn model"));
    CHECK(hasRow(", / .", "Select mesh"));
    CHECK(hasRow("F7/F8", "Select material"));
    CHECK(hasRow("PgUp", "Select previous"));
    CHECK(hasRow("PgDn", "Select next"));
    CHECK(hasRow("H / L", "Move X"));
    CHECK(hasRow("J / K", "Move Z"));
    CHECK(hasRow("Home", "Move Y down"));
    CHECK(hasRow("End", "Move Y up"));
    CHECK(hasRow("1/2/3", "Rotation axis"));
    CHECK(hasRow("X / V", "Rotate"));
    CHECK(hasRow("F9/F10", "Scale"));
    CHECK(hasRow("Delete", "Remove object"));
    CHECK(hasRow("F6", "Save scene"));
}
