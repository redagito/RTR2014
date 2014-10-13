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

    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        LOG_ERROR("Failed to open json file %s.", file.c_str());
        return false;
    }

    if (!reader.parse(ifs, root))
    {
        LOG_ERROR("Failed to load scene file %s with error %s.", file.c_str(),
                  reader.getFormattedErrorMessages().c_str());
        return false;
    }
    ifs.close();

    Json::Value sceneObjects = root["scene_objects"];
    if (root.empty())
    {
        LOG_ERROR(
            "Missing or empty 'scene_objects' node in json file %s. No scene objects are loaded.",
            file.c_str());
        return false;
    }

    if (!sceneObjects.isArray())
    {
        LOG_ERROR("In json file %s, the node 'scene_objects' must be array type.", file.c_str());
        return false;
    }

    for (unsigned int i = 0; i < sceneObjects.size(); ++i)
    {
        Json::Value sceneObject = sceneObjects[i];
        Json::Value mesh = sceneObject["mesh"];
        if (mesh.empty())
        {
            LOG_ERROR("In json file %s, undefined 'mesh' parameter.", file.c_str());
            return false;
        }

        Json::Value material = sceneObject["material"];
        if (material.empty())
        {
            LOG_ERROR("In json file %s, undefined 'material' parameter.", file.c_str());
            return false;
        }

        Json::Value position = sceneObject["position"];
        if (position.empty())
        {
            LOG_ERROR("In json file %s, undefined 'position' parameter.", file.c_str());
            return false;
        }

        Json::Value rotation = sceneObject["rotation"];
        if (rotation.empty())
        {
            LOG_ERROR("In json file %s, undefined 'rotation' parameter.", file.c_str());
            return false;
        }

        Json::Value scale = sceneObject["scale"];
        if (scale.empty())
        {
            LOG_ERROR("In json file %s, undefined 'scale' parameter.", file.c_str());
            return false;
        }

        if (position.size() != 3)
        {
            LOG_ERROR("In json file %s, the 'position' parameter does not have size 3.",
                      file.c_str());
            return false;
        }
        if (rotation.size() != 3)
        {
            LOG_ERROR("In json file %s, the 'rotation' parameter does not have size 3.",
                      file.c_str());
            return false;
        }
        if (scale.size() != 3)
        {
            LOG_ERROR("In json file %s, the 'scale' parameter does not have size 3.", file.c_str());
            return false;
        }

        if (!position[0].isNumeric() || !position[1].isNumeric() || !position[2].isNumeric())
        {
            LOG_ERROR("In json file %s, the 'position' parameter contains a non-numeric value.",
                      file.c_str());
            return false;
        }
        if (!rotation[0].isNumeric() || !rotation[1].isNumeric() || !rotation[2].isNumeric())
        {
            LOG_ERROR("In json file %s, the 'rotation' parameter contains a non-numeric value.",
                      file.c_str());
            return false;
        }
        if (!scale[0].isNumeric() || !scale[1].isNumeric() || !scale[2].isNumeric())
        {
            LOG_ERROR("In json file %s, the 'scale' parameter contains a non-numeric value.",
                      file.c_str());
            return false;
        }

        ResourceId meshId = m_resourceManager.loadMesh(mesh.asString());
        if (meshId == -1)
        {
            LOG_ERROR("In json file %s, failed to load mesh file %s.", file.c_str(),
                      mesh.asCString());
            return false;
        }

        ResourceId materialId = m_resourceManager.loadMaterial(material.asString());
        if (materialId == -1)
        {
            LOG_ERROR("In json file %s, failed to load material file %s.", file.c_str(),
                      material.asCString());
            return false;
        }

        glm::vec3 positionVec(position[0].asFloat(), position[1].asFloat(), position[2].asFloat());
        glm::vec3 rotationVec(rotation[0].asFloat(), rotation[1].asFloat(), rotation[2].asFloat());
        glm::vec3 scaleVec(scale[0].asFloat(), scale[1].asFloat(), scale[2].asFloat());

        // Create object in scene
        scene.createObject(meshId, materialId, positionVec, rotationVec, scaleVec);
    }

    return true;
}