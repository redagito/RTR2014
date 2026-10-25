#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Json
{
class Value;
}

/**
 * \brief Data of a scene object that is written to a scene file.
 */
struct SceneObjectRecord
{
    std::string m_mesh;
    std::string m_material;
    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale = glm::vec3(1.f);
    bool m_castsShadow = true;
};

/**
 * \brief Writes scene objects back to a scene file.
 *
 * The file is read once, so that every other node of the scene (lights, sky,
 * music, animations and sounds) is preserved. Every save replaces the objects
 * that were added since the file was opened, which keeps repeated saves from
 * duplicating them.
 */
class SceneWriter
{
   public:
    SceneWriter();
    ~SceneWriter();

    /**
     * \brief Reads the scene file and remembers its current scene objects.
     */
    bool open(const std::string& file);

    /**
     * \brief Writes the file with the given objects appended to the original ones.
     *
     * A backup of the previous file is written next to it with the suffix .bak.
     */
    bool save(const std::vector<SceneObjectRecord>& objects);

    /**
     * \brief The file the writer was opened with.
     */
    const std::string& getFile() const;

   private:
    std::string m_file;
    std::unique_ptr<Json::Value> m_root;
    size_t m_baseObjectCount = 0; /**< Scene objects that were in the file when it was opened. */
    bool m_opened = false;
};
