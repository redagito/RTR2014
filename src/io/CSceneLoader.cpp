#include "CSceneLoader.h"

#include <fstream>

#include <json/json.h>
#include <glm/glm.hpp>

#include "graphics/IScene.h"
#include "resource/IResourceManager.h"

#include "JsonDeserializer.h"

#include "debug/Log.h"

#include "animation/CRotationController.h"
#include "animation/CMovementController.h"

CSceneLoader::CSceneLoader(IResourceManager& resourceManager) : m_resourceManager(resourceManager)
{
    return;
}

bool CSceneLoader::load(const std::string& file, IScene& scene,
                        std::vector<std::shared_ptr<IAnimationController>>& controllers)
{
    Json::Reader reader;
    Json::Value root;

    // Load scene file
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        LOG_ERROR("Failed to open json file %s.", file.c_str());
        return false;
    }

    // Parse json data
    if (!reader.parse(ifs, root))
    {
        LOG_ERROR("Failed to load scene file %s with error %s.", file.c_str(),
                  reader.getFormattedErrorMessages().c_str());
        return false;
    }
    // Read done, close file
    ifs.close();

    // Load scene objects
    if (!loadSceneObjects(root["scene_objects"], scene, controllers))
    {
        LOG_ERROR("Error while loading scene objects from scene file %s.", file.c_str());
        return false;
    }

    // Load point lights
    if (!loadPointLights(root["point_lights"], scene, controllers))
    {
        LOG_ERROR("Error while loading point lights from scene file %s.", file.c_str());
        return false;
    }

    // Load directional lights
    if (!loadDirectionalLights(root["directional_lights"], scene, controllers))
    {
        LOG_ERROR("Error while loading ambient light from scene file &s.", file.c_str());
        return false;
    }

    // Load ambient light
    if (!loadAmbientLight(root["ambient_light"], scene))
    {
        LOG_ERROR("Error while loading ambient light from scene file &s.", file.c_str());
        return false;
    }

    return true;
}

bool CSceneLoader::loadSceneObjects(const Json::Value& node, IScene& scene,
                                    std::vector<std::shared_ptr<IAnimationController>>& controllers)
{
    // Node empty?
    if (node.empty())
    {
        LOG_INFO("Missing or empty 'scene_objects' node. No scene objects are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        LOG_ERROR("The node 'scene_objects' must be array type.");
        return false;
    }

    // Load scene objects
    for (unsigned int i = 0; i < node.size(); ++i)
    {
        if (!loadSceneObject(node[i], scene, controllers))
        {
            return false;
        }
    }
    return true;
}

bool CSceneLoader::loadSceneObject(const Json::Value& node, IScene& scene,
                                   std::vector<std::shared_ptr<IAnimationController>>& controllers)
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
        LOG_ERROR("Failed to load mesh file %s.", mesh.c_str());
        return false;
    }

    // Load material file
    ResourceId materialId = m_resourceManager.loadMaterial(material);
    if (materialId == -1)
    {
        LOG_ERROR("Failed to load material file %s.", material.c_str());
        return false;
    }

    // Create object in scene
    SceneObjectId objectId = scene.createObject(meshId, materialId, position, rotation, scale);

    // Load optional animation controllers
    if (!loadAnimationControllers(node["animations"], scene, controllers, objectId,
                                  AnimationObjectType::Model))
    {
        LOG_ERROR("Failed to load animation controller.");
        return false;
    }

    return true;
}

bool CSceneLoader::loadPointLights(const Json::Value& node, IScene& scene,
                                   std::vector<std::shared_ptr<IAnimationController>>& controllers)
{
    // Node empty?
    if (node.empty())
    {
        LOG_INFO("Missing or empty 'point_lights' node. No point lights are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        LOG_ERROR("The node 'point_lights' must be array type.");
        return false;
    }

    // Load scene point lights
    for (unsigned int i = 0; i < node.size(); ++i)
    {
        if (!loadPointLight(node[i], scene, controllers))
        {
            return false;
        }
    }
    return true;
}

bool CSceneLoader::loadPointLight(const Json::Value& node, IScene& scene,
                                  std::vector<std::shared_ptr<IAnimationController>>& controllers)
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
	SceneObjectId objectId = scene.createPointLight(position, radius, color, intensity, castsShadow);

    // Load optional animation controllers
    if (!loadAnimationControllers(node["animations"], scene, controllers, objectId,
                                  AnimationObjectType::PointLight))
    {
        LOG_ERROR("Failed to load animation controller.");
        return false;
    }
    return true;
}

bool CSceneLoader::loadDirectionalLights(
    const Json::Value& node, IScene& scene,
    std::vector<std::shared_ptr<IAnimationController>>& controllers)
{
    // Node empty?
    if (node.empty())
    {
        LOG_INFO("Missing or empty 'directional_lights' node. No directional lights are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        LOG_ERROR("The node 'directional_lights' must be array type.");
        return false;
    }

    // Load scene directional lights
    for (unsigned int i = 0; i < node.size(); ++i)
    {
        if (!loadDirectionalLight(node[i], scene, controllers))
        {
            return false;
        }
    }
    return true;
}

bool CSceneLoader::loadDirectionalLight(
    const Json::Value& node, IScene& scene,
    std::vector<std::shared_ptr<IAnimationController>>& controllers)
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
        LOG_INFO("Missing or empty 'ambient_light' node. No ambient light is loaded.");
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

bool CSceneLoader::loadAnimationControllers(
    const Json::Value& node, IScene& scene,
    std::vector<std::shared_ptr<IAnimationController>>& controllers, SceneObjectId id,
    AnimationObjectType type)
{
    if (node.empty())
    {
        // LOG_INFO("Missing or empty 'animations' node. No animation controllers where loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        LOG_ERROR("The node 'animations' must be array type.");
        return false;
    }

    // Load animation controllers for object
    for (unsigned int i = 0; i < node.size(); ++i)
    {
        if (!loadAnimationController(node[i], scene, controllers, id, type))
        {
            return false;
        }
    }
    return true;
}

bool CSceneLoader::loadAnimationController(
    const Json::Value& node, IScene& scene,
    std::vector<std::shared_ptr<IAnimationController>>& controllers, SceneObjectId id,
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
        controllers.push_back(std::make_shared<CRotationController>(id, type, scene, rotation));
    }
    else if (controllerType == "movement")
    {
        glm::vec3 direction;
        if (!load(node, "direction", direction))
        {
            return false;
        }
        controllers.push_back(
            std::make_shared<CMovementController>(id, type, scene, direction));
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
    if (!deserialize(node[name], f))
    {
        LOG_ERROR("Failed to load '%s' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool CSceneLoader::load(const Json::Value& node, const std::string& name, bool& b)
{
    if (!deserialize(node[name], b))
    {
        LOG_ERROR("Failed to load '%s' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool CSceneLoader::load(const Json::Value& node, const std::string& name, glm::vec3& vec)
{
    if (!deserialize(node[name], vec))
    {
        LOG_ERROR("Failed to load '%s' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool CSceneLoader::load(const Json::Value& node, const std::string& name, std::string& str)
{
    if (!deserialize(node[name], str))
    {
        LOG_ERROR("Failed to load '%s' parameter.", name.c_str());
        return false;
    }
    return true;
}