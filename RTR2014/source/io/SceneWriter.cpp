#include "io/SceneWriter.h"

#include <json/json.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <system_error>

namespace
{
Json::Value toJson(const glm::vec3& vec)
{
    Json::Value node(Json::arrayValue);
    node.append(vec.x);
    node.append(vec.y);
    node.append(vec.z);
    return node;
}

bool backupFile(const std::filesystem::path& file)
{
    std::error_code error;
    if (!std::filesystem::exists(file, error))
    {
        // Nothing to back up on the first save.
        return true;
    }

    const std::filesystem::path backup = file.string() + ".bak";
    std::filesystem::copy_file(file, backup, std::filesystem::copy_options::overwrite_existing,
                               error);
    if (error)
    {
        spdlog::error("Failed to create the scene backup {}: {}.", backup.string(),
                      error.message());
        return false;
    }
    return true;
}
}  // namespace

SceneWriter::SceneWriter() {}

SceneWriter::~SceneWriter() {}

bool SceneWriter::open(const std::string& file)
{
    m_opened = false;
    m_file = file;
    m_root.reset();
    m_baseObjectCount = 0;

    Json::Reader reader;
    Json::Value root;
    std::ifstream input(file);
    if (!input.is_open())
    {
        spdlog::error("Failed to open scene file {} for saving.", file.c_str());
        return false;
    }
    if (!reader.parse(input, root))
    {
        spdlog::error("Failed to parse scene file {}: {}.", file.c_str(),
                      reader.getFormattedErrorMessages().c_str());
        return false;
    }
    if (!root.isObject())
    {
        spdlog::error("The root of scene file {} must be an object.", file.c_str());
        return false;
    }

    const Json::Value& objects = root["scene_objects"];
    if (!objects.isNull() && !objects.isArray())
    {
        spdlog::error("The 'scene_objects' node of scene file {} must be an array.", file.c_str());
        return false;
    }
    m_baseObjectCount = objects.size();

    m_root = std::make_unique<Json::Value>(std::move(root));
    m_opened = true;
    return true;
}

bool SceneWriter::save(const std::vector<SceneObjectRecord>& objects)
{
    if (!m_opened || m_root == nullptr)
    {
        spdlog::error("The scene writer has to open a file before it can save.");
        return false;
    }

    Json::Value& sceneObjects = (*m_root)["scene_objects"];
    if (!sceneObjects.isArray())
    {
        sceneObjects = Json::Value(Json::arrayValue);
    }
    // Drop the objects of the previous save, the original ones stay in place.
    sceneObjects.resize(static_cast<Json::ArrayIndex>(m_baseObjectCount));

    for (const SceneObjectRecord& record : objects)
    {
        Json::Value node;
        node["mesh"] = record.m_mesh;
        node["material"] = record.m_material;
        node["position"] = toJson(record.m_position);
        node["rotation"] = toJson(record.m_rotation);
        node["scale"] = toJson(record.m_scale);
        node["casts_shadow"] = record.m_castsShadow;
        sceneObjects.append(std::move(node));
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "    ";
    const std::string text = Json::writeString(builder, *m_root);

    const std::filesystem::path file(m_file);
    if (!backupFile(file))
    {
        return false;
    }

    std::ofstream output(file, std::ios::binary | std::ios::trunc);
    if (!output.is_open())
    {
        spdlog::error("Failed to open scene file {} for writing.", m_file.c_str());
        return false;
    }
    output << text;
    output.close();
    if (!output)
    {
        spdlog::error("Failed to write scene file {}.", m_file.c_str());
        return false;
    }

    spdlog::info("Saved the scene to {}.", m_file.c_str());
    return true;
}

const std::string& SceneWriter::getFile() const { return m_file; }
