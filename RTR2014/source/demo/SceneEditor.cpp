#include "demo/SceneEditor.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <locale>
#include <sstream>
#include <system_error>
#include <utility>

#include "graphics/IScene.h"
#include "graphics/camera/IControllableCamera.h"
#include "input/IInputProvider.h"
#include "resource/IResourceManager.h"

namespace
{
constexpr float kSpawnDistance = 3.f;
constexpr float kMoveStep = 0.1f;
constexpr float kRotationStep = glm::radians(5.f);
constexpr float kScaleStep = 0.1f;
constexpr float kMoveRepeat = 0.08f;
constexpr float kSelectionRepeat = 0.25f;
constexpr float kActionRepeat = 0.4f;
constexpr float kTwoPi = 2.f * 3.14159265358979323846f;

std::string formatFloat(float value, int precision)
{
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

std::string formatVector(const glm::vec3& vec)
{
    return "(" + formatFloat(vec.x, 2) + ", " + formatFloat(vec.y, 2) + ", " +
           formatFloat(vec.z, 2) + ")";
}

std::string formatScale(const glm::vec3& scale) { return formatFloat(scale.x, 2); }
}  // namespace

SceneEditor::SceneEditor(IScene& scene, IControllableCamera& camera, IInputProvider& inputProvider,
                         IResourceManager& resourceManager, const std::string& sceneFile,
                         const std::string& meshDirectory, const std::string& materialDirectory)
    : m_scene(scene),
      m_camera(camera),
      m_inputProvider(inputProvider),
      m_resourceManager(resourceManager)
{
    if (!m_writer.open(sceneFile))
    {
        spdlog::error("The scene editor cannot save to {}.", sceneFile.c_str());
    }
    scanAssets(meshDirectory, materialDirectory);
    m_status = "Ready";
}

SceneEditor::~SceneEditor() {}

void SceneEditor::update(float timeDiff)
{
    // The editor keys must stay disjoint from the demo and camera bindings
    // (DemoController, CameraController and RTRDemo), so editing never triggers
    // a camera or renderer action. German QWERTZ swaps Y and Z and moves the
    // punctuation keys, so layout-stable keys are used here.
    if (pressed(GLFW_KEY_TAB, kActionRepeat, timeDiff))
    {
        m_active = !m_active;
        m_status = m_active ? "On" : "Off";
    }
    if (!m_active)
    {
        return;
    }

    if (pressed(GLFW_KEY_ENTER, kActionRepeat, timeDiff))
    {
        spawnSelected();
    }
    if (pressed(GLFW_KEY_F6, kActionRepeat, timeDiff))
    {
        saveScene();
    }

    if (pressed(GLFW_KEY_COMMA, kSelectionRepeat, timeDiff))
    {
        selectMesh(-1);
    }
    if (pressed(GLFW_KEY_PERIOD, kSelectionRepeat, timeDiff))
    {
        selectMesh(1);
    }
    if (pressed(GLFW_KEY_F7, kSelectionRepeat, timeDiff))
    {
        selectMaterial(-1);
    }
    if (pressed(GLFW_KEY_F8, kSelectionRepeat, timeDiff))
    {
        selectMaterial(1);
    }
    if (pressed(GLFW_KEY_PAGE_UP, kSelectionRepeat, timeDiff))
    {
        selectObject(-1);
    }
    if (pressed(GLFW_KEY_PAGE_DOWN, kSelectionRepeat, timeDiff))
    {
        selectObject(1);
    }
    if (pressed(GLFW_KEY_DELETE, kActionRepeat, timeDiff))
    {
        removeSelected();
    }

    if (pressed(GLFW_KEY_H, kMoveRepeat, timeDiff))
    {
        moveSelected(glm::vec3(-kMoveStep, 0.f, 0.f));
    }
    if (pressed(GLFW_KEY_L, kMoveRepeat, timeDiff))
    {
        moveSelected(glm::vec3(kMoveStep, 0.f, 0.f));
    }
    if (pressed(GLFW_KEY_J, kMoveRepeat, timeDiff))
    {
        moveSelected(glm::vec3(0.f, 0.f, -kMoveStep));
    }
    if (pressed(GLFW_KEY_K, kMoveRepeat, timeDiff))
    {
        moveSelected(glm::vec3(0.f, 0.f, kMoveStep));
    }
    if (pressed(GLFW_KEY_HOME, kMoveRepeat, timeDiff))
    {
        moveSelected(glm::vec3(0.f, -kMoveStep, 0.f));
    }
    if (pressed(GLFW_KEY_END, kMoveRepeat, timeDiff))
    {
        moveSelected(glm::vec3(0.f, kMoveStep, 0.f));
    }

    if (pressed(GLFW_KEY_1, kSelectionRepeat, timeDiff))
    {
        m_rotationAxis = 0;
    }
    if (pressed(GLFW_KEY_2, kSelectionRepeat, timeDiff))
    {
        m_rotationAxis = 1;
    }
    if (pressed(GLFW_KEY_3, kSelectionRepeat, timeDiff))
    {
        m_rotationAxis = 2;
    }
    if (pressed(GLFW_KEY_X, kMoveRepeat, timeDiff))
    {
        rotateSelected(-kRotationStep);
    }
    if (pressed(GLFW_KEY_V, kMoveRepeat, timeDiff))
    {
        rotateSelected(kRotationStep);
    }

    if (pressed(GLFW_KEY_F9, kMoveRepeat, timeDiff))
    {
        scaleSelected(-kScaleStep);
    }
    if (pressed(GLFW_KEY_F10, kMoveRepeat, timeDiff))
    {
        scaleSelected(kScaleStep);
    }
}

bool SceneEditor::isActive() const { return m_active; }

bool SceneEditor::pressed(int keyCode, float repeatInterval, float timeDiff)
{
    float& cooldown = m_keyCooldowns[keyCode];
    cooldown -= timeDiff;
    if (!m_inputProvider.isKeyPressed(keyCode) || cooldown > 0.f)
    {
        return false;
    }
    cooldown = repeatInterval;
    return true;
}

void SceneEditor::scanAssets(const std::string& meshDirectory, const std::string& materialDirectory)
{
    const auto collect = [](const std::string& directory, const std::string& extension)
    {
        std::vector<std::string> files;
        const std::filesystem::path path(directory);
        std::error_code error;
        if (!std::filesystem::is_directory(path, error))
        {
            spdlog::warn("The scene editor asset directory {} does not exist.", directory.c_str());
            return files;
        }

        for (const std::filesystem::directory_entry& entry :
             std::filesystem::directory_iterator(path, error))
        {
            if (error)
            {
                spdlog::error("Failed to list scene editor assets in {}: {}.", directory.c_str(),
                              error.message());
                break;
            }
            if (!entry.is_regular_file() || entry.path().extension() != extension)
            {
                continue;
            }
            files.push_back((path / entry.path().filename()).generic_string());
        }
        std::sort(files.begin(), files.end());
        return files;
    };

    m_meshFiles = collect(meshDirectory, ".obj");
    m_materialFiles = collect(materialDirectory, ".ini");
    m_meshIndex = 0;
    m_materialIndex = 0;

    spdlog::info("The scene editor found {} meshes and {} materials.", m_meshFiles.size(),
                 m_materialFiles.size());
}

bool SceneEditor::spawnSelected()
{
    if (m_meshFiles.empty())
    {
        m_status = "No mesh files";
        spdlog::warn("The scene editor has no mesh files to spawn.");
        return false;
    }
    if (m_materialFiles.empty())
    {
        m_status = "No materials";
        spdlog::warn("The scene editor has no material files to spawn.");
        return false;
    }

    const std::string& meshFile = m_meshFiles[m_meshIndex];
    const std::string& materialFile = m_materialFiles[m_materialIndex];

    const ResourceId meshId = m_resourceManager.loadMesh(meshFile);
    if (meshId == invalidResource)
    {
        m_status = "Mesh failed";
        spdlog::error("The scene editor failed to load mesh {}.", meshFile.c_str());
        return false;
    }

    const ResourceId materialId = m_resourceManager.loadMaterial(materialFile);
    if (materialId == invalidResource)
    {
        m_status = "Material failed";
        spdlog::error("The scene editor failed to load material {}.", materialFile.c_str());
        return false;
    }

    OrientedBoundingBox bounds;
    if (!m_resourceManager.getMeshBounds(meshId, bounds))
    {
        m_status = "Bounds failed";
        spdlog::error("The scene editor failed to get the bounds of mesh {}.", meshFile.c_str());
        return false;
    }

    const glm::mat4& view = m_camera.getView();
    glm::vec3 forward = -glm::vec3(view[0][2], view[1][2], view[2][2]);
    if (glm::dot(forward, forward) < 1e-8f)
    {
        forward = glm::vec3(0.f, 0.f, -1.f);
    }
    forward = glm::normalize(forward);

    EditorObject object;
    object.m_id =
        m_scene.createObject(meshId, materialId, m_camera.getPosition() + forward * kSpawnDistance,
                             glm::vec3(0.f), glm::vec3(1.f), bounds);
    object.m_meshFile = meshFile;
    object.m_materialFile = materialFile;
    m_objects.push_back(std::move(object));
    m_selection = m_objects.size() - 1;
    m_status = "Spawned " + fileName(meshFile);

    spdlog::info("The scene editor spawned {} with material {}.", meshFile.c_str(),
                 materialFile.c_str());
    return true;
}

void SceneEditor::removeSelected()
{
    EditorObject* object = getSelectedObject();
    if (object == nullptr)
    {
        m_status = "Nothing selected";
        return;
    }

    spdlog::info("The scene editor removed {}.", object->m_meshFile.c_str());
    m_scene.removeObject(object->m_id);
    m_objects.erase(m_objects.begin() + static_cast<std::ptrdiff_t>(m_selection));
    if (m_objects.empty())
    {
        m_selection = 0;
    }
    else if (m_selection >= m_objects.size())
    {
        m_selection = m_objects.size() - 1;
    }
    m_status = "Removed object";
}

void SceneEditor::moveSelected(const glm::vec3& offset)
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    if (!getSelectedTransform(position, rotation, scale))
    {
        return;
    }
    setSelectedTransform(position + offset, rotation, scale);
    m_status = "Moved object";
}

void SceneEditor::rotateSelected(float amount)
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    if (!getSelectedTransform(position, rotation, scale))
    {
        return;
    }
    float& angle = rotation[m_rotationAxis];
    angle = std::fmod(angle + amount, kTwoPi);
    if (angle < 0.f)
    {
        angle += kTwoPi;
    }
    setSelectedTransform(position, rotation, scale);
    m_status = "Rotated object";
}

void SceneEditor::scaleSelected(float amount)
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    if (!getSelectedTransform(position, rotation, scale))
    {
        return;
    }
    scale = glm::max(scale * (1.f + amount), glm::vec3(0.01f));
    setSelectedTransform(position, rotation, scale);
    m_status = "Scaled object";
}

void SceneEditor::selectObject(int direction)
{
    if (m_objects.empty())
    {
        return;
    }
    const int count = static_cast<int>(m_objects.size());
    const int next = (static_cast<int>(m_selection) + direction + count) % count;
    m_selection = static_cast<std::size_t>(next);
}

void SceneEditor::selectMesh(int direction)
{
    if (m_meshFiles.empty())
    {
        return;
    }
    const int count = static_cast<int>(m_meshFiles.size());
    const int next = (static_cast<int>(m_meshIndex) + direction + count) % count;
    m_meshIndex = static_cast<std::size_t>(next);
}

void SceneEditor::selectMaterial(int direction)
{
    if (m_materialFiles.empty())
    {
        return;
    }
    const int count = static_cast<int>(m_materialFiles.size());
    const int next = (static_cast<int>(m_materialIndex) + direction + count) % count;
    m_materialIndex = static_cast<std::size_t>(next);
}

SceneEditor::EditorObject* SceneEditor::getSelectedObject()
{
    if (m_objects.empty() || m_selection >= m_objects.size())
    {
        return nullptr;
    }
    return &m_objects[m_selection];
}

const SceneEditor::EditorObject* SceneEditor::getSelectedObject() const
{
    if (m_objects.empty() || m_selection >= m_objects.size())
    {
        return nullptr;
    }
    return &m_objects[m_selection];
}

bool SceneEditor::getSelectedTransform(glm::vec3& position, glm::vec3& rotation,
                                       glm::vec3& scale) const
{
    const EditorObject* object = getSelectedObject();
    if (object == nullptr)
    {
        return false;
    }

    ResourceId mesh;
    ResourceId material;
    bool castsShadow = true;
    return m_scene.getObject(object->m_id, mesh, material, position, rotation, scale, castsShadow);
}

void SceneEditor::setSelectedTransform(const glm::vec3& position, const glm::vec3& rotation,
                                       const glm::vec3& scale)
{
    const EditorObject* object = getSelectedObject();
    if (object == nullptr)
    {
        return;
    }

    ResourceId mesh;
    ResourceId material;
    glm::vec3 ignoredPosition;
    glm::vec3 ignoredRotation;
    glm::vec3 ignoredScale;
    bool castsShadow = true;
    if (!m_scene.getObject(object->m_id, mesh, material, ignoredPosition, ignoredRotation,
                           ignoredScale, castsShadow))
    {
        return;
    }
    m_scene.setObject(object->m_id, mesh, material, position, rotation, scale);
}

bool SceneEditor::saveScene()
{
    std::vector<SceneObjectRecord> records;
    records.reserve(m_objects.size());
    for (const EditorObject& object : m_objects)
    {
        ResourceId mesh;
        ResourceId material;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        bool castsShadow = true;
        if (!m_scene.getObject(object.m_id, mesh, material, position, rotation, scale, castsShadow))
        {
            continue;
        }
        records.push_back(
            {object.m_meshFile, object.m_materialFile, position, rotation, scale, castsShadow});
    }

    if (!m_writer.save(records))
    {
        m_status = "Save failed";
        return false;
    }

    m_status = "Saved " + fileName(m_writer.getFile());
    return true;
}

void SceneEditor::appendDebugInfo(std::vector<DebugInfo::Value>& values) const
{
    const auto shortcut = [](std::string key, std::string setting, std::string status = {})
    {
        return DebugInfo::Value{std::move(key), std::move(setting), std::move(status),
                                DebugInfo::ValueKind::Shortcut};
    };
    const auto state = [](std::string key, std::string setting, std::string status)
    {
        return DebugInfo::Value{std::move(key), std::move(setting), std::move(status),
                                DebugInfo::ValueKind::State};
    };

    const auto currentMesh = [this]()
    {
        if (m_meshFiles.empty())
        {
            return std::string("None");
        }
        return shorten(std::to_string(m_meshIndex + 1) + "/" + std::to_string(m_meshFiles.size()) +
                           " " + fileName(m_meshFiles[m_meshIndex]),
                       20);
    };
    const auto currentMaterial = [this]()
    {
        if (m_materialFiles.empty())
        {
            return std::string("None");
        }
        return shorten(std::to_string(m_materialIndex + 1) + "/" +
                           std::to_string(m_materialFiles.size()) + " " +
                           fileName(m_materialFiles[m_materialIndex]),
                       20);
    };

    const std::string selection = m_objects.empty() ? std::string("None")
                                                    : std::to_string(m_selection + 1) + "/" +
                                                          std::to_string(m_objects.size());

    values.push_back(shortcut("Tab", "Close editor", "On"));
    values.push_back(shortcut("Enter", "Spawn model", currentMesh()));
    values.push_back(shortcut(", / .", "Select mesh", currentMesh()));
    values.push_back(shortcut("F7/F8", "Select material", currentMaterial()));
    values.push_back(shortcut("PgUp", "Select previous", selection));
    values.push_back(shortcut("PgDn", "Select next", selection));
    values.push_back(shortcut("H / L", "Move X", "left / right"));
    values.push_back(shortcut("J / K", "Move Z", "forward / back"));
    values.push_back(shortcut("Home", "Move Y down"));
    values.push_back(shortcut("End", "Move Y up"));

    const char* axes[] = {"X", "Y", "Z"};
    values.push_back(shortcut("1/2/3", "Rotation axis", axes[m_rotationAxis]));
    values.push_back(shortcut("X / V", "Rotate", "-5 / +5 deg"));
    values.push_back(shortcut("F9/F10", "Scale", "-10 / +10 %"));
    values.push_back(shortcut("Delete", "Remove object"));
    values.push_back(shortcut("F6", "Save scene", shorten(m_status, 20)));

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    if (getSelectedTransform(position, rotation, scale))
    {
        values.push_back(state("Position", "Selected", formatVector(position)));
        values.push_back(state("Rotation", "Selected", formatVector(rotation)));
        values.push_back(state("Scale", "Selected", formatScale(scale)));
    }
    else
    {
        values.push_back(state("Position", "No selection", "Spawn a model"));
    }
}

std::string SceneEditor::fileName(const std::string& path)
{
    const std::size_t separator = path.find_last_of("/\\");
    return separator == std::string::npos ? path : path.substr(separator + 1);
}

std::string SceneEditor::shorten(const std::string& text, std::size_t length)
{
    if (text.size() <= length || length < 3)
    {
        return text;
    }
    return text.substr(0, length - 2) + "..";
}
