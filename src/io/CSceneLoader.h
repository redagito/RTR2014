#pragma once

#include <string>

#include "graphics/IScene.h"
#include "resource/IResourceManager.h"

namespace Json
{
class Value; /**< Forward declare for json node handling. */
}

/**
* \brief Scene loader utility class.
* Populates scene with static geometry from file.
* Used for testing only, ids are not stored.
*/
class CSceneLoader
{
   public:
    CSceneLoader(IResourceManager& resourceManager);
    bool load(const std::string& file, IScene& scene);

   protected:
    bool loadSceneObjects(const Json::Value& node, IScene& scene);
    bool loadSceneObject(const Json::Value& node, IScene& scene);

    bool loadPointLights(const Json::Value& node, IScene& scene);
    bool loadPointLight(const Json::Value& node, IScene& scene);

   private:
    IResourceManager& m_resourceManager;
};