#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "resource/ResourceConfig.h"
#include "resource/ResourceManager.h"

namespace
{
class TempObj
{
   public:
    explicit TempObj(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempObj() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_meshloader.obj";
};

struct LoadedMesh
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    PrimitiveType type = PrimitiveType::Invalid;
};

LoadedMesh loadMesh(const std::string& file)
{
    ResourceManager manager;
    const ResourceId id = manager.loadMesh(file);

    LoadedMesh mesh;
    REQUIRE(id != invalidResource);
    REQUIRE(manager.getMesh(id, mesh.vertices, mesh.indices, mesh.normals, mesh.uvs, mesh.type));
    return mesh;
}
}  // namespace

TEST_CASE("a triangle face keeps its three corners", "[mesh]")
{
    TempObj file(
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f 1 2 3\n");

    const LoadedMesh mesh = loadMesh(file.path());

    REQUIRE(mesh.type == PrimitiveType::Triangle);
    REQUIRE(mesh.vertices.size() == 9);
}

TEST_CASE("a quad face is triangulated into two triangles", "[mesh]")
{
    TempObj file(
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 1 1 0\n"
        "v 0 1 0\n"
        "f 1 2 3 4\n");

    const LoadedMesh mesh = loadMesh(file.path());

    REQUIRE(mesh.type == PrimitiveType::Triangle);
    REQUIRE(mesh.vertices.size() == 18);
}

TEST_CASE("a pentagon face is triangulated into three triangles", "[mesh]")
{
    TempObj file(
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 2 1 0\n"
        "v 1 2 0\n"
        "v 0 2 0\n"
        "f 1 2 3 4 5\n");

    const LoadedMesh mesh = loadMesh(file.path());

    REQUIRE(mesh.type == PrimitiveType::Triangle);
    REQUIRE(mesh.vertices.size() == 27);
}

TEST_CASE("the vertices of a triangulated face keep their source order", "[mesh]")
{
    TempObj file(
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 1 1 0\n"
        "v 0 1 0\n"
        "f 1 2 3 4\n");

    const LoadedMesh mesh = loadMesh(file.path());

    const std::vector<float> expected = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f,
                                         0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f};
    REQUIRE(mesh.vertices == expected);
}

TEST_CASE("a face with relative indices is resolved", "[mesh]")
{
    TempObj file(
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f -3 -2 -1\n");

    const LoadedMesh mesh = loadMesh(file.path());

    REQUIRE(mesh.vertices.size() == 9);
}

TEST_CASE("a mesh file without any face is rejected", "[mesh]")
{
    TempObj file(
        "v 0 0 0\n"
        "v 1 0 0\n"
        "f 1 2\n");

    ResourceManager manager;

    REQUIRE(manager.loadMesh(file.path()) == invalidResource);
}

TEST_CASE("the sword mesh is triangulated into the expected number of triangles", "[mesh]")
{
    // The sword is made of 1812 quads and 10 pentagons, which fan triangulate
    // into 3654 triangles.
    const LoadedMesh mesh = loadMesh(RTR_PROJECT_DATA_DIR "/mesh/sword.obj");

    REQUIRE(mesh.type == PrimitiveType::Triangle);
    REQUIRE(mesh.vertices.size() == 3654 * 3 * 3);
}
