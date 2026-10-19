#include <spdlog/spdlog.h>
#include "io/CSceneLoader.h"

#include <fstream>

#include <json/json.h>
#include <glm/glm.hpp>

#include "graphics/IScene.h"
#include "resource/IResourceManager.h"

#include "io/JsonDeserializer.h"


#include "animation/CAnimationWorld.h"
#include "animation/CRotationController.h"
#include "animation/CMovementController.h"

CSceneLoader::CSceneLoader(IResourceManager& resourceManager) : m_resourceManager(resourceManager)
{
    return;
}

bool CSceneLoader::load(const std::string& file, IScene& scene, CAnimationWorld& animationWorld)
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

    // Load scene objects
    if (!loadSceneObjects(root["scene_objects"], scene, animationWorld))
    {
        spdlog::error("Error while loading scene objects from scene file {}.", file.c_str());
        return false;
    }

    // Load point lights
    if (!loadPointLights(root["point_lights"], scene, animationWorld))
    {
        spdlog::error("Error while loading point lights from scene file {}.", file.c_str());
        return false;
    }

    // Load directional lights
    if (!loadDirectionalLights(root["directional_lights"], scene, animationWorld))
    {
        spdlog::error("Error while loading ambient light from scene file &s.", file.c_str());
        return false;
    }

    // Load ambient light
    if (!loadAmbientLight(root["ambient_light"], scene))
    {
        spdlog::error("Error while loading ambient light from scene file &s.", file.c_str());
        return false;
    }

    return true;
}

bool CSceneLoader::loadSceneObjects(const Json::Value& node, IScene& scene,
                                    CAnimationWorld& animationWorld)
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

bool CSceneLoader::loadSceneObject(const Json::Value& node, IScene& scene,
                                   CAnimationWorld& animationWorld)
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

bool CSceneLoader::loadPointLights(const Json::Value& node, IScene& scene,
                                   CAnimationWorld& animationWorld)
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

bool CSceneLoader::loadPointLight(const Json::Value& node, IScene& scene,
                                  CAnimationWorld& animationWorld)
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

bool CSceneLoader::loadDirectionalLights(const Json::Value& node, IScene& scene,
                                         CAnimationWorld& animationWorld)
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

bool CSceneLoader::loadDirectionalLight(const Json::Value& node, IScene& scene,
                                        CAnimationWorld& animationWorld)
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

bool CSceneLoader::loadAmbientLight(const Json::Value& node, IScene& scene)
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

bool CSceneLoader::loadAnimationControllers(const Json::Value& node, IScene& scene,
                                            CAnimationWorld& animationWorld, SceneObjectId id,
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

bool CSceneLoader::loadAnimationController(const Json::Value& node, IScene& scene,
                                           CAnimationWorld& animationWorld, SceneObjectId id,
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
		animationWorld.addAnimationController(std::make_shared<CRotationController>(id, type, scene, rotation));
    }
    else if (controllerType == "movement")
    {
        glm::vec3 direction;
        if (!load(node, "direction", direction))
        {
            return false;
        }
		animationWorld.addAnimationController(std::make_shared<CMovementController>(id, type, scene, direction));
    }
    else
    {
        // Unknown controller type
        return false;
    }

    return true;
}

bool CSceneLoader::load(const Json::Value& node, const std::string& name, float& f)
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

bool CSceneLoader::load(const Json::Value& node, const std::string& name, bool& b)
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

bool CSceneLoader::load(const Json::Value& node, const std::string& name, glm::vec3& vec)
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

bool CSceneLoader::load(const Json::Value& node, const std::string& name, std::string& str)
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
