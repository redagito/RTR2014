#pragma once

#include <string>

#include "graphics/IScene.h"
#include "graphics/IResourceManager.h"

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

   private:
    IResourceManager& m_resourceManager;
};