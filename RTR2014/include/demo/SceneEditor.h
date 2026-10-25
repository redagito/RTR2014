#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "debug/DebugInfo.h"
#include "graphics/SceneConfig.h"
#include "io/SceneWriter.h"

class IControllableCamera;
class IInputProvider;
class IResourceManager;
class IScene;

/**
 * \brief Keyboard driven scene editor.
 *
 * Spawns models from the mesh and material folders into the running scene,
 * lets the user adjust the transform of the models it spawned and saves them
 * back to the scene file. It does not touch the objects that were loaded from
 * the file.
 */
class SceneEditor
{
   public:
    SceneEditor(IScene& scene, IControllableCamera& camera, IInputProvider& inputProvider,
                IResourceManager& resourceManager, const std::string& sceneFile,
                const std::string& meshDirectory = "data/mesh",
                const std::string& materialDirectory = "data/material");
    ~SceneEditor();

    /**
     * \brief Handles the editor keys for this frame.
     */
    void update(float timeDiff);

    /**
     * \brief Whether the editor is currently active.
     */
    bool isActive() const;

    /**
     * \brief Appends the editor rows to the debug overlay.
     */
    void appendDebugInfo(std::vector<DebugInfo::Value>& values) const;

    /**
     * \brief Writes the spawned objects to the scene file.
     */
    bool saveScene();

   private:
    struct EditorObject
    {
        SceneObjectId m_id = invalidObject;
        std::string m_meshFile;
        std::string m_materialFile;
    };

    /**
     * \brief Finds the mesh and material files to choose from.
     */
    void scanAssets(const std::string& meshDirectory, const std::string& materialDirectory);

    /**
     * \brief Returns true once per press and repeat interval of a key.
     */
    bool pressed(int keyCode, float repeatInterval, float timeDiff);

    /**
     * \brief Creates the currently selected mesh and material in front of the camera.
     */
    bool spawnSelected();

    /**
     * \brief Removes the selected editor object from the scene.
     */
    void removeSelected();

    /**
     * \brief Changes the transform of the selected editor object.
     */
    void moveSelected(const glm::vec3& offset);
    void rotateSelected(float amount);
    void scaleSelected(float amount);

    /**
     * \brief Selects the previous or next editor object.
     */
    void selectObject(int direction);

    void selectMesh(int direction);
    void selectMaterial(int direction);

    EditorObject* getSelectedObject();
    const EditorObject* getSelectedObject() const;

    /**
     * \brief Reads the transform of the selected object.
     */
    bool getSelectedTransform(glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) const;

    /**
     * \brief Writes the transform of the selected object.
     */
    void setSelectedTransform(const glm::vec3& position, const glm::vec3& rotation,
                              const glm::vec3& scale);

    static std::string fileName(const std::string& path);
    static std::string shorten(const std::string& text, size_t length);

    IScene& m_scene;
    IControllableCamera& m_camera;
    IInputProvider& m_inputProvider;
    IResourceManager& m_resourceManager;
    SceneWriter m_writer;

    std::vector<std::string> m_meshFiles;
    std::vector<std::string> m_materialFiles;
    std::size_t m_meshIndex = 0;
    std::size_t m_materialIndex = 0;

    std::vector<EditorObject> m_objects;
    std::size_t m_selection = 0;
    int m_rotationAxis = 1; /**< 0 = X, 1 = Y, 2 = Z. */

    bool m_active = false;
    std::string m_status;
    std::unordered_map<int, float> m_keyCooldowns;
};
