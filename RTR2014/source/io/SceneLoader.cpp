#include <spdlog/spdlog.h>
#include "io/SceneLoader.h"

#include <fstream>

#include <json/json.h>
#include <glm/glm.hpp>

#include "graphics/IScene.h"
#include "resource/IResourceManager.h"

#include "io/JsonDeserializer.h"


#include "animation/AnimationWorld.h"
#include "animation/RotationController.h"
#include "animation/MovementController.h"

SceneLoader::SceneLoader(IResourceManager& resourceManager) : m_resourceManager(resourceManager)
{
    return;
}

SceneLoader::~SceneLoader()
{
    return;
}

bool SceneLoader::prepare(const std::string& file)
{
    Json::Reader reader;
    Json::Value root;

    // Load scene file
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        spdlog::error("Failed to open json file {}.", file.c_str());
        return false;
    }

    // Parse json data
    if (!reader.parse(ifs, root))
    {
        spdlog::error("Failed to load scene file {} with error {}.", file.c_str(),
                   reader.getFormattedErrorMessages().c_str());
        return false;
    }
    // Read done, close file
    ifs.close();

    if (!root.isObject())
    {
        spdlog::error("The root of scene file {} must be an object.", file.c_str());
        return false;
    }

    // Load the referenced resources in parallel before publishing them
    std::vector<std::string> meshes;
    std::vector<std::string> materials;
    if (!collectResources(root["scene_objects"], meshes, materials))
    {
        spdlog::error("The scene file {} has an invalid scene objects node.", file.c_str());
        return false;
    }
    m_resourceManager.preload(meshes, materials);

    m_root = std::make_unique<Json::Value>(std::move(root));
    return true;
}

bool SceneLoader::load(IScene& scene, AnimationWorld& animationWorld)
{
    if (m_root == nullptr)
    {
        spdlog::error("The scene must be prepared before it can be loaded.");
        return false;
    }
    const Json::Value& root = *m_root;

    // Load scene objects
    if (!loadSceneObjects(root["scene_objects"], scene, animationWorld))
    {
        spdlog::error("Error while loading scene objects from scene file.");
        return false;
    }

    // Load point lights
    if (!loadPointLights(root["point_lights"], scene, animationWorld))
    {
        spdlog::error("Error while loading point lights from scene file.");
        return false;
    }

    // Load directional lights
    if (!loadDirectionalLights(root["directional_lights"], scene, animationWorld))
    {
        spdlog::error("Error while loading directional lights from scene file.");
        return false;
    }

    // Load ambient light
    if (!loadAmbientLight(root["ambient_light"], scene))
    {
        spdlog::error("Error while loading ambient light from scene file.");
        return false;
    }

    return true;
}

bool SceneLoader::loadSceneObjects(const Json::Value& node, IScene& scene,
                                    AnimationWorld& animationWorld)
{
    // Node empty?
    if (node.empty())
    {
        spdlog::info("Missing or empty 'scene_objects' node. No scene objects are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        spdlog::error("The node 'scene_objects' must be array type.");
        return false;
    }

    // Load scene objects
    for (unsigned int i = 0; i < node.size(); ++i)
    {
        if (!loadSceneObject(node[i], scene, animationWorld))
        {
            return false;
        }
    }
    return true;
}

bool SceneLoader::collectResources(const Json::Value& node, std::vector<std::string>& meshes,
                                   std::vector<std::string>& materials)
{
    if (node.empty())
    {
        return true;
    }
    if (!node.isArray())
    {
        spdlog::error("The node 'scene_objects' must be array type.");
        return false;
    }
    for (unsigned int i = 0; i < node.size(); ++i)
    {
        if (!node[i].isObject())
        {
            spdlog::error("A scene object must be an object node.");
            return false;
        }
        std::string mesh;
        if (node[i].isMember("mesh") && deserialize(node[i]["mesh"], mesh))
        {
            meshes.push_back(mesh);
        }
        std::string material;
        if (node[i].isMember("material") && deserialize(node[i]["material"], material))
        {
            materials.push_back(material);
        }
    }
    return true;
}

bool SceneLoader::loadSceneObject(const Json::Value& node, IScene& scene,
                                   AnimationWorld& animationWorld)
{
    std::string mesh;
    std::string material;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    if (!load(node, "mesh", mesh))
    {
        return false;
    }

    if (!load(node, "material", material))
    {
        return false;
    }

    if (!load(node, "position", position))
    {
        return false;
    }

    if (!load(node, "rotation", rotation))
    {
        return false;
    }

    if (!load(node, "scale", scale))
    {
        return false;
    }

    // Load mesh file
    ResourceId meshId = m_resourceManager.loadMesh(mesh);
    if (meshId == -1)
    {
        spdlog::error("Failed to load mesh file {}.", mesh.c_str());
        return false;
    }

    // Load material file
    ResourceId materialId = m_resourceManager.loadMaterial(material);
    if (materialId == -1)
    {
        spdlog::error("Failed to load material file {}.", material.c_str());
        return false;
    }

    // Create object in scene
    SceneObjectId objectId = scene.createObject(meshId, materialId, position, rotation, scale);

    // Load optional animation controllers
	if (!loadAnimationControllers(node["animations"], scene, animationWorld, objectId,
                                  AnimationObjectType::Model))
    {
        spdlog::error("Failed to load animation controller.");
        return false;
    }

    return true;
}

bool SceneLoader::loadPointLights(const Json::Value& node, IScene& scene,
                                   AnimationWorld& animationWorld)
{
    // Node empty?
    if (node.empty())
    {
        spdlog::info("Missing or empty 'point_lights' node. No point lights are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        spdlog::error("The node 'point_lights' must be array type.");
        return false;
    }

    // Load scene point lights
    for (unsigned int i = 0; i < node.size(); ++i)
    {
		if (!loadPointLight(node[i], scene, animationWorld))
        {
            return false;
        }
    }
    return true;
}

bool SceneLoader::loadPointLight(const Json::Value& node, IScene& scene,
                                  AnimationWorld& animationWorld)
{
    glm::vec3 position;
    glm::vec3 color;
    float radius;
    float intensity;
    bool castsShadow;

    if (!load(node, "position", position))
    {
        return false;
    }

    if (!load(node, "color", color))
    {
        return false;
    }

    if (!load(node, "radius", radius))
    {
        return false;
    }

    if (!load(node, "intensity", intensity))
    {
        return false;
    }

    if (!load(node, "casts_shadow", castsShadow))
    {
        return false;
    }

    // Create object in scene
    SceneObjectId objectId =
        scene.createPointLight(position, radius, color, intensity, castsShadow);

    // Load optional animation controllers
	if (!loadAnimationControllers(node["animations"], scene, animationWorld, objectId,
                                  AnimationObjectType::PointLight))
    {
        spdlog::error("Failed to load animation controller.");
        return false;
    }
    return true;
}

bool SceneLoader::loadDirectionalLights(const Json::Value& node, IScene& scene,
                                         AnimationWorld& animationWorld)
{
    // Node empty?
    if (node.empty())
    {
        spdlog::info("Missing or empty 'directional_lights' node. No directional lights are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        spdlog::error("The node 'directional_lights' must be array type.");
        return false;
    }

    // Load scene directional lights
    for (unsigned int i = 0; i < node.size(); ++i)
    {
		if (!loadDirectionalLight(node[i], scene, animationWorld))
        {
            return false;
        }
    }
    return true;
}

bool SceneLoader::loadDirectionalLight(const Json::Value& node, IScene& scene,
                                        AnimationWorld& animationWorld)
{
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    bool castsShadow;

    if (!load(node, "direction", direction))
    {
        return false;
    }

    if (!load(node, "color", color))
    {
        return false;
    }

    if (!load(node, "intensity", intensity))
    {
        return false;
    }

    if (!load(node, "casts_shadow", castsShadow))
    {
        return false;
    }

    // Create object in scene
    scene.createDirectionalLight(direction, color, intensity, castsShadow);
    return true;
}

bool SceneLoader::loadAmbientLight(const Json::Value& node, IScene& scene)
{
    if (node.empty())
    {
        spdlog::info("Missing or empty 'ambient_light' node. No ambient light is loaded.");
        return true;
    }

    glm::vec3 color;
    float intensity;

    if (!load(node, "color", color))
    {
        return false;
    }

    if (!load(node, "intensity", intensity))
    {
        return false;
    }

    scene.setAmbientLight(color, intensity);
    return true;
}

bool SceneLoader::loadAnimationControllers(const Json::Value& node, IScene& scene,
                                            AnimationWorld& animationWorld, SceneObjectId id,
                                            AnimationObjectType type)
{
    if (node.empty())
    {
        // spdlog::info("Missing or empty 'animations' node. No animation controllers where loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        spdlog::error("The node 'animations' must be array type.");
        return false;
    }

    // Load animation controllers for object
    for (unsigned int i = 0; i < node.size(); ++i)
    {
		if (!loadAnimationController(node[i], scene, animationWorld, id, type))
        {
            return false;
        }
    }
    return true;
}

bool SceneLoader::loadAnimationController(const Json::Value& node, IScene& scene,
                                           AnimationWorld& animationWorld, SceneObjectId id,
                                           AnimationObjectType type)
{
    std::string controllerType;
    if (!load(node, "type", controllerType))
    {
        return false;
    }
    //
    if (controllerType == "rotation")
    {
        glm::vec3 rotation;
        if (!load(node, "rotation", rotation))
        {
            return false;
        }
		animationWorld.addAnimationController(std::make_shared<RotationController>(id, type, scene, rotation));
    }
    else if (controllerType == "movement")
    {
        glm::vec3 direction;
        if (!load(node, "direction", direction))
        {
            return false;
        }
		animationWorld.addAnimationController(std::make_shared<MovementController>(id, type, scene, direction));
    }
    else
    {
        // Unknown controller type
        return false;
    }

    return true;
}

bool SceneLoader::load(const Json::Value& node, const std::string& name, float& f)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], f))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool SceneLoader::load(const Json::Value& node, const std::string& name, bool& b)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], b))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool SceneLoader::load(const Json::Value& node, const std::string& name, glm::vec3& vec)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], vec))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool SceneLoader::load(const Json::Value& node, const std::string& name, std::string& str)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], str))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}
