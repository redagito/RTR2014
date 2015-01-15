#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "animation/Animation.h"
#include "graphics/SceneConfig.h"

namespace Json
{
class Value; /**< Forward declare for json node handling. */
}

class IScene;
class IResourceManager;
class IAnimationController;

/**
* \brief Scene loader utility class.
* Populates scene with static geometry from file.
* Used for testing only, ids are not stored.
*/
class CSceneLoader
{
   public:
    /**
    * \brief Create scene loader with resource manager.
    */
    CSceneLoader(IResourceManager& resourceManager);
    bool load(const std::string& file, IScene& scene,
              std::vector<std::shared_ptr<IAnimationController>>& controllers);

   protected:
    bool loadSceneObjects(const Json::Value& node, IScene& scene,
                          std::vector<std::shared_ptr<IAnimationController>>& controllers);
    bool loadSceneObject(const Json::Value& node, IScene& scene,
                         std::vector<std::shared_ptr<IAnimationController>>& controllers);

    bool loadPointLights(const Json::Value& node, IScene& scene,
                         std::vector<std::shared_ptr<IAnimationController>>& controllers);
    bool loadPointLight(const Json::Value& node, IScene& scene,
                        std::vector<std::shared_ptr<IAnimationController>>& controllers);

    bool loadDirectionalLights(const Json::Value& node, IScene& scene,
                               std::vector<std::shared_ptr<IAnimationController>>& controllers);
    bool loadDirectionalLight(const Json::Value& node, IScene& scene,
                              std::vector<std::shared_ptr<IAnimationController>>& controllers);

    bool loadAmbientLight(const Json::Value& node, IScene& scene);

	bool loadAnimationControllers(const Json::Value& node, IScene& scene,
		std::vector<std::shared_ptr<IAnimationController>>& controllers, SceneObjectId id, AnimationObjectType type);

	bool loadAnimationController(const Json::Value& node, IScene& scene,
		std::vector<std::shared_ptr<IAnimationController>>& controllers, SceneObjectId id, AnimationObjectType type);

    bool load(const Json::Value& node, const std::string& name, float& f);
    bool load(const Json::Value& node, const std::string& name, bool& b);
    bool load(const Json::Value& node, const std::string& name, glm::vec3& vec);
    bool load(const Json::Value& node, const std::string& name, std::string& str);

   private:
    IResourceManager& m_resourceManager;
};