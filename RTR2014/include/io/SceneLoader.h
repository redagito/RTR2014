#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "animation/Animation.h"
#include "graphics/SceneConfig.h"
#include "resource/ResourceConfig.h"

namespace Json
{
class Value; /**< Forward declare for json node handling. */
}

class IScene;
class IResourceManager;
class AnimationWorld;

/**
* \brief Scene loader utility class.
* Populates scene with static geometry from file.
* Used for testing only, ids are not stored.
*/
class SceneLoader
{
   public:
     /**
    * \brief Create scene loader with resource manager.
    */
     SceneLoader(IResourceManager& resourceManager);

     /**
    * \brief Releases the parsed scene file.
    */
     ~SceneLoader();

     /**
    * \brief Parses the scene file and decodes the resources it references.
    *
    * Does not touch the graphics context, so it can run on a worker thread.
    */
     bool prepare(const std::string& file);

     /**
    * \brief Loads the prepared scene.
    *
    * Creates the graphics resources, so it must run on the thread owning the
    * graphics context.
    */
     bool load(IScene& scene, AnimationWorld& animationWorld);

   protected:
    bool loadSceneObjects(const Json::Value& node, IScene& scene, AnimationWorld& animationWorld);
    bool loadSceneObject(const Json::Value& node, IScene& scene, AnimationWorld& animationWorld);

    bool loadPointLights(const Json::Value& node, IScene& scene, AnimationWorld& animationWorld);
    bool loadPointLight(const Json::Value& node, IScene& scene, AnimationWorld& animationWorld);

    bool loadDirectionalLights(const Json::Value& node, IScene& scene,
                               AnimationWorld& animationWorld);
    bool loadDirectionalLight(const Json::Value& node, IScene& scene,
                              AnimationWorld& animationWorld);

    bool loadAmbientLight(const Json::Value& node, IScene& scene);

    bool loadAnimationControllers(const Json::Value& node, IScene& scene,
                                  AnimationWorld& animationWorld, SceneObjectId id,
                                  AnimationObjectType type);

    bool loadAnimationController(const Json::Value& node, IScene& scene,
                                 AnimationWorld& animationWorld, SceneObjectId id,
                                 AnimationObjectType type);

      bool load(const Json::Value& node, const std::string& name, float& f);
      bool load(const Json::Value& node, const std::string& name, bool& b);
      bool load(const Json::Value& node, const std::string& name, glm::vec3& vec);
      bool load(const Json::Value& node, const std::string& name, std::string& str);

      /**
      * \brief Collects the mesh and material files of the scene objects.
      */
      bool collectResources(const Json::Value& node, std::vector<std::string>& meshes,
                            std::vector<std::string>& materials);

     private:
      IResourceManager& m_resourceManager;

      std::unique_ptr<Json::Value> m_root; /**< Parsed scene file. */
  };
