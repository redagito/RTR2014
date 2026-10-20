#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

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
 * \brief A positional sound source of a loaded scene.
 */
struct SoundEmitter
{
    ResourceId m_sound = invalidResource;
    glm::vec3 m_position = glm::vec3(0.0f);
    float m_radius = 0.0f;
};

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

    /**
     * \brief Resource id of the background music published by the last load.
     */
    ResourceId getMusicResourceId() const;

    /**
     * \brief Sound emitters published by the last load.
     */
    const std::vector<SoundEmitter>& getSoundEmitters() const;

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
    struct PendingSoundEmitter
    {
        std::string m_file;
        glm::vec3 m_position = glm::vec3(0.0f);
        float m_radius = 0.0f;
    };

    /**
     * \brief Validates the 'sound_emitters' node of the scene file.
     */
    bool parseSoundEmitters(const Json::Value& node, std::vector<PendingSoundEmitter>& emitters);

    IResourceManager& m_resourceManager;

    std::unique_ptr<Json::Value> m_root; /**< Parsed scene file. */
    std::string m_musicFile;             /**< Background music of the prepared scene. */
    ResourceId m_musicResourceId = invalidResource;
    std::vector<PendingSoundEmitter> m_pendingEmitters;
    std::vector<SoundEmitter> m_emitters;
};
