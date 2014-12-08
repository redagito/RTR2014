#include "CSceneLoader.h"

#include <fstream>

#include <json/json.h>
#include <glm/glm.hpp>

#include "debug/Log.h"

CSceneLoader::CSceneLoader(IResourceManager& resourceManager) : m_resourceManager(resourceManager)
{
    return;
}

bool CSceneLoader::load(const std::string& file, IScene& scene)
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
    if (!loadSceneObjects(root["scene_objects"], scene))
    {
        LOG_ERROR("Error while loading scene objects from scene file %s.", file.c_str());
        return false;
    }

	// Load point lights
	if (!loadPointLights(root["scene_point_lights"], scene))
	{
		LOG_ERROR("Error while loading point lights from scene file %s.", file.c_str());
		return false;
	}
    return true;
}

bool CSceneLoader::loadSceneObjects(const Json::Value& node, IScene& scene)
{
    // Node empty?
    if (node.empty())
    {
        LOG_ERROR("Missing or empty 'scene_objects' node. No scene objects are loaded.");
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
        if (!loadSceneObject(node[i], scene))
        {
            return false;
        }
    }
	return true;
}

bool CSceneLoader::loadSceneObject(const Json::Value& node, IScene& scene)
{
    // Mesh parameter
    Json::Value mesh = node["mesh"];
    if (mesh.empty())
    {
        LOG_ERROR("Missing 'mesh' parameter.");
        return false;
    }

    // Material parameter
    Json::Value material = node["material"];
    if (material.empty())
    {
        LOG_ERROR("Missing 'material' parameter.");
        return false;
    }

    // Position parameter
    Json::Value position = node["position"];
    if (position.empty())
    {
        LOG_ERROR("Missing 'position' parameter.");
        return false;
    }

    // Rotation parameter
    Json::Value rotation = node["rotation"];
    if (rotation.empty())
    {
        LOG_ERROR("Missing 'rotation' parameter.");
        return false;
    }

    // Scale parameter
    Json::Value scale = node["scale"];
    if (scale.empty())
    {
        LOG_ERROR("Missing 'scale' parameter.");
        return false;
    }

    // Check parameter size
    if (position.size() != 3)
    {
        LOG_ERROR("The 'position' parameter does not have size 3.");
        return false;
    }
    if (rotation.size() != 3)
    {
        LOG_ERROR("The 'rotation' parameter does not have size 3.");
        return false;
    }
    if (scale.size() != 3)
    {
        LOG_ERROR("The 'scale' parameter does not have size 3.");
        return false;
    }

    // Check parameter types
    if (!position[0].isNumeric() || !position[1].isNumeric() || !position[2].isNumeric())
    {
        LOG_ERROR("The 'position' parameter contains a non-numeric value.");
        return false;
    }
    if (!rotation[0].isNumeric() || !rotation[1].isNumeric() || !rotation[2].isNumeric())
    {
        LOG_ERROR("The 'rotation' parameter contains a non-numeric value.");
        return false;
    }
    if (!scale[0].isNumeric() || !scale[1].isNumeric() || !scale[2].isNumeric())
    {
        LOG_ERROR("The 'scale' parameter contains a non-numeric value.");
        return false;
    }

	// Load mesh file
    ResourceId meshId = m_resourceManager.loadMesh(mesh.asString());
    if (meshId == -1)
    {
        LOG_ERROR("Failed to load mesh file %s.", mesh.asCString());
        return false;
    }

	// Load material file
    ResourceId materialId = m_resourceManager.loadMaterial(material.asString());
    if (materialId == -1)
    {
        LOG_ERROR("Failed to load material file %s.", material.asCString());
        return false;
    }

	// Load transformations
    glm::vec3 positionVec(position[0].asFloat(), position[1].asFloat(), position[2].asFloat());
    glm::vec3 rotationVec(rotation[0].asFloat(), rotation[1].asFloat(), rotation[2].asFloat());
    glm::vec3 scaleVec(scale[0].asFloat(), scale[1].asFloat(), scale[2].asFloat());

    // Create object in scene
    scene.createObject(meshId, materialId, positionVec, rotationVec, scaleVec);
	return true;
}

bool CSceneLoader::loadPointLights(const Json::Value& node, IScene& scene) 
{ 
	// Node empty?
	if (node.empty())
	{
		LOG_ERROR("Missing or empty 'scene_point_lights' node. No point lights are loaded.");
		return true;
	}

	// Node is array type
	if (!node.isArray())
	{
		LOG_ERROR("The node 'scene_point_lights' must be array type.");
		return false;
	}

	// Load scene point lights
	for (unsigned int i = 0; i < node.size(); ++i)
	{
		if (!loadPointLight(node[i], scene))
		{
			return false;
		}
	}
	return true;
}

bool CSceneLoader::loadPointLight(const Json::Value& node, IScene& scene)
{
	// Position parameter
	Json::Value position = node["position"];
	if (position.empty())
	{
		LOG_ERROR("Missing 'position' parameter.");
		return false;
	}

	// Color parameter
	Json::Value color = node["color"];
	if (color.empty())
	{
		LOG_ERROR("Missing 'color' parameter.");
		return false;
	}

	// Radius parameter
	Json::Value radius = node["radius"];
	if (radius.empty())
	{
		LOG_ERROR("Missing 'radius' parameter.");
		return false;
	}

	// Intensity parameter
	Json::Value intensity = node["intensity"];
	if (intensity.empty())
	{
		LOG_ERROR("Missing 'intensity' parameter.");
		return false;
	}

	// Check parameter size
	if (position.size() != 3)
	{
		LOG_ERROR("The 'position' parameter does not have size 3.");
		return false;
	}
	if (color.size() != 3)
	{
		LOG_ERROR("The 'color' parameter does not have size 3.");
		return false;
	}

	// Check parameter types
	if (!position[0].isNumeric() || !position[1].isNumeric() || !position[2].isNumeric())
	{
		LOG_ERROR("The 'position' parameter contains a non-numeric value.");
		return false;
	}
	if (!color[0].isNumeric() || !color[1].isNumeric() || !color[2].isNumeric())
	{
		LOG_ERROR("The 'color' parameter contains a non-numeric value.");
		return false;
	}
	if (!radius.isNumeric())
	{
		LOG_ERROR("The 'radius' parameter contains a non-numeric value.");
		return false;
	}
	if (!intensity.isNumeric())
	{
		LOG_ERROR("The 'intensity' parameter contains a non-numeric value.");
		return false;
	}

	// Light position
	glm::vec3 positionVec(position[0].asFloat(), position[1].asFloat(), position[2].asFloat());
	// Light parameters
	glm::vec3 colorVec(color[0].asFloat(), color[1].asFloat(), color[2].asFloat());
	float radiusVal = radius.asFloat();
	float intensityVal = intensity.asFloat();

	// Create object in scene
	scene.createPointLight(positionVec, radiusVal, colorVec, intensityVal);
	return true;
}