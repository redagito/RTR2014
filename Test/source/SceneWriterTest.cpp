#include <json/json.h>

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "io/SceneWriter.h"

namespace
{
class TempSceneFile
{
   public:
    explicit TempSceneFile(const std::string& content)
    {
        std::ofstream out(m_path);
        out << content;
    }

    ~TempSceneFile()
    {
        std::remove(m_path.c_str());
        std::remove((m_path + ".bak").c_str());
    }

    const std::string& path() const { return m_path; }
    std::string backupPath() const { return m_path + ".bak"; }

   private:
    std::string m_path = "rtr2014_tmp_writer_scene.json";
};

Json::Value readJson(const std::string& file)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream input(file);
    REQUIRE(reader.parse(input, root));
    return root;
}
}  // namespace

TEST_CASE("SceneWriter appends objects and preserves the other nodes", "[io]")
{
    TempSceneFile file(
        "{\"music\": \"keep.mp3\", \"scene_objects\": "
        "[{\"mesh\": \"original.obj\", \"material\": \"original.ini\"}]}");

    SceneWriter writer;
    REQUIRE(writer.open(file.path()));

    SceneObjectRecord record;
    record.m_mesh = "data/mesh/cube.obj";
    record.m_material = "data/material/test.ini";
    record.m_position = glm::vec3(1.f, 2.f, 3.f);
    record.m_rotation = glm::vec3(0.5f, 0.f, 0.f);
    record.m_scale = glm::vec3(2.f);
    record.m_castsShadow = false;
    REQUIRE(writer.save({record}));

    const Json::Value root = readJson(file.path());
    REQUIRE(root["music"].asString() == "keep.mp3");
    const Json::Value& objects = root["scene_objects"];
    REQUIRE(objects.size() == 2);
    CHECK(objects[0]["mesh"].asString() == "original.obj");
    CHECK(objects[1]["mesh"].asString() == "data/mesh/cube.obj");
    CHECK(objects[1]["material"].asString() == "data/material/test.ini");
    CHECK(objects[1]["position"][0].asFloat() == 1.f);
    CHECK(objects[1]["position"][1].asFloat() == 2.f);
    CHECK(objects[1]["position"][2].asFloat() == 3.f);
    CHECK(objects[1]["scale"][0].asFloat() == 2.f);
    CHECK_FALSE(objects[1]["casts_shadow"].asBool());
}

TEST_CASE("SceneWriter replaces the objects of its previous save", "[io]")
{
    TempSceneFile file("{\"scene_objects\": []}");

    SceneWriter writer;
    REQUIRE(writer.open(file.path()));

    SceneObjectRecord first;
    first.m_mesh = "first.obj";
    first.m_material = "first.ini";
    REQUIRE(writer.save({first}));

    SceneObjectRecord second;
    second.m_mesh = "second.obj";
    second.m_material = "second.ini";
    REQUIRE(writer.save({second}));

    const Json::Value root = readJson(file.path());
    REQUIRE(root["scene_objects"].size() == 1);
    CHECK(root["scene_objects"][0]["mesh"].asString() == "second.obj");
}

TEST_CASE("SceneWriter writes a backup of the previous scene", "[io]")
{
    TempSceneFile file("{\"scene_objects\": []}");

    SceneWriter writer;
    REQUIRE(writer.open(file.path()));
    REQUIRE(writer.save({}));

    const Json::Value backup = readJson(file.backupPath());
    REQUIRE(backup["scene_objects"].isArray());
    CHECK(backup["scene_objects"].empty());
}

TEST_CASE("SceneWriter rejects a file it cannot read", "[io]")
{
    SceneWriter writer;
    REQUIRE_FALSE(writer.open("does_not_exist_writer_scene.json"));
    REQUIRE_FALSE(writer.save({}));
}
