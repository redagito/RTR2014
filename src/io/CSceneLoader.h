#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Json
{
class Value; /**< Forward declare for json node handling. */
}

class IScene;
class IResourceManager;

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
    bool load(const std::string& file, IScene& scene);

   protected:
    bool loadSceneObjects(const Json::Value& node, IScene& scene);
    bool loadSceneObject(const Json::Value& node, IScene& scene);

    bool loadPointLights(const Json::Value& node, IScene& scene);
    bool loadPointLight(const Json::Value& node, IScene& scene);

	bool loadDirectionalLights(const Json::Value& node, IScene& scene);
	bool loadDirectionalLight(const Json::Value& node, IScene& scene);

	bool loadAmbientLight(const Json::Value& node, IScene& scene);

	bool load(const Json::Value& node, const std::string& name, float& f);
	bool load(const Json::Value& node, const std::string& name, glm::vec3& vec);
	bool load(const Json::Value& node, const std::string& name, std::string& str);

   private:
    IResourceManager& m_resourceManager;
};