#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

#include "animation/AnimationWorld.h"
#include "graphics/scene/Scene.h"
#include "io/SceneLoader.h"
#include "resource/ResourceManager.h"

namespace
{
class TempJson
{
   public:
    explicit TempJson(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempJson() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_scene.json";
};
}  // namespace

TEST_CASE("SceneLoader rejects a scene object that is not an object node", "[io]")
{
    TempJson file("{\"scene_objects\": [42]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a scene whose root is an array", "[io]")
{
    TempJson file("[1, 2, 3]");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a missing file", "[io]")
{
    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare("does_not_exist_scene.json"));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader loads music and sound emitters", "[io]")
{
    TempJson file(std::string("{\"music\": \"") +
                  RTR_TEST_DATA_DIR
                  "/tone.mp3\","
                  "\"scene_objects\": [],"
                  "\"sound_emitters\": [{\"sound\": \"" +
                  RTR_TEST_DATA_DIR
                  "/tone.mp3\","
                  "\"position\": [-8, 0.5, 31], \"radius\": 10}]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE(loader.prepare(file.path()));
    REQUIRE(loader.load(scene, animationWorld));

    REQUIRE(loader.getMusicResourceId() != invalidResource);
    REQUIRE(resourceManager.getSound(loader.getMusicResourceId()) != nullptr);

    const std::vector<SoundEmitter>& emitters = loader.getSoundEmitters();
    REQUIRE(emitters.size() == 1);
    CHECK(emitters[0].m_sound != invalidResource);
    REQUIRE(resourceManager.getSound(emitters[0].m_sound) != nullptr);
    CHECK(emitters[0].m_position == glm::vec3(-8.0f, 0.5f, 31.0f));
    CHECK(emitters[0].m_radius == 10.0f);
}

TEST_CASE("SceneLoader reports no music without a music node", "[io]")
{
    TempJson file("{\"scene_objects\": []}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE(loader.prepare(file.path()));
    REQUIRE(loader.load(scene, animationWorld));

    CHECK(loader.getMusicResourceId() == invalidResource);
    CHECK(loader.getSoundEmitters().empty());
}

TEST_CASE("SceneLoader rejects a sound emitters node that is not an array", "[io]")
{
    TempJson file("{\"scene_objects\": [], \"sound_emitters\": {}}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a sound emitter without a sound node", "[io]")
{
    TempJson file(
        "{\"scene_objects\": [],"
        "\"sound_emitters\": [{\"position\": [0, 0, 0], \"radius\": 5}]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a sound emitter with an invalid position", "[io]")
{
    TempJson file(std::string("{\"scene_objects\": [],"
                              "\"sound_emitters\": [{\"sound\": \"") +
                  RTR_TEST_DATA_DIR
                  "/tone.mp3\","
                  "\"position\": \"here\", \"radius\": 5}]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a sound emitter with a non-positive radius", "[io]")
{
    TempJson file(std::string("{\"scene_objects\": [],"
                              "\"sound_emitters\": [{\"sound\": \"") +
                  RTR_TEST_DATA_DIR
                  "/tone.mp3\","
                  "\"position\": [0, 0, 0], \"radius\": 0}]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader skips an emitter whose sound file cannot be loaded", "[io]")
{
    TempJson file(
        "{\"scene_objects\": [],"
        "\"sound_emitters\": [{\"sound\": \"does_not_exist_sound.mp3\","
        "\"position\": [1, 2, 3], \"radius\": 5}]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE(loader.prepare(file.path()));
    REQUIRE(loader.load(scene, animationWorld));
    CHECK(loader.getSoundEmitters().empty());
}

TEST_CASE("SceneLoader decodes the color of the lights", "[io]")
{
    TempJson file(
        "{\"scene_objects\": [],"
        "\"ambient_light\": {\"color\": [0.5, 0.25, 1.0], \"intensity\": 1.0},"
        "\"directional_lights\": [{\"direction\": [0, -1, 0],"
        "\"color\": [0.5, 0.25, 1.0], \"intensity\": 1.0}]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE(loader.prepare(file.path()));
    REQUIRE(loader.load(scene, animationWorld));

    const glm::vec3 expected = glm::pow(glm::vec3(0.5f, 0.25f, 1.0f), glm::vec3(2.2f));

    glm::vec3 ambientColor;
    float ambientIntensity = 0.0f;
    REQUIRE(scene.getAmbientLight(ambientColor, ambientIntensity));
    CHECK(ambientColor == expected);

    glm::vec3 direction;
    glm::vec3 color;
    float intensity = 0.0f;
    bool castsShadow = false;
    REQUIRE(scene.getDirectionalLight(0, direction, color, intensity, castsShadow));
    CHECK(color == expected);
}