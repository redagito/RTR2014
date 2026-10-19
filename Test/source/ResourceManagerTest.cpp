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
    std::string m_path = "rtr2014_tmp_preload.obj";
};

class TempIni
{
   public:
    explicit TempIni(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempIni() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_preload.ini";
};

// The same object loaded with and without preloading must yield the same data.
const char* kObj = "v 0 0 0\n"
                    "v 1 0 0\n"
                    "v 0 1 0\n"
                    "vn 0 0 1\n"
                    "vt 0 0\n"
                    "f 1/1/1 2/1/1 3/1/1\n";
} // namespace

TEST_CASE("loadImage rejects a file that is not a png", "[resource]")
{
    ResourceManager manager;

    REQUIRE(manager.loadImage("not_an_image.txt", ColorFormat::RGBA32) == invalidResource);
}

TEST_CASE("loadImage rejects an unknown color format", "[resource]")
{
    ResourceManager manager;

    REQUIRE(manager.loadImage("image.png", ColorFormat::Invalid) == invalidResource);
}

TEST_CASE("preloaded mesh data matches the synchronously loaded data", "[resource]")
{
    TempObj file(kObj);

    ResourceManager preloaded;
    preloaded.preload({file.path()}, {});
    const ResourceId preloadedId = preloaded.loadMesh(file.path());

    ResourceManager synchronous;
    const ResourceId synchronousId = synchronous.loadMesh(file.path());

    REQUIRE(preloadedId != invalidResource);
    REQUIRE(synchronousId != invalidResource);

    std::vector<float> preloadedVertices;
    std::vector<unsigned int> preloadedIndices;
    std::vector<float> preloadedNormals;
    std::vector<float> preloadedUvs;
    PrimitiveType preloadedType = PrimitiveType::Invalid;
    REQUIRE(preloaded.getMesh(preloadedId, preloadedVertices, preloadedIndices, preloadedNormals,
                              preloadedUvs, preloadedType));

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    PrimitiveType type = PrimitiveType::Invalid;
    REQUIRE(synchronous.getMesh(synchronousId, vertices, indices, normals, uvs, type));

    REQUIRE(preloadedVertices == vertices);
    REQUIRE(preloadedNormals == normals);
    REQUIRE(preloadedUvs == uvs);
    REQUIRE(preloadedType == type);
}

TEST_CASE("preloading an unknown mesh file does not publish it", "[resource]")
{
    ResourceManager manager;
    manager.preload({"no_such_file.obj"}, {});

    REQUIRE(manager.loadMesh("no_such_file.obj") == invalidResource);
}

TEST_CASE("preloading an unknown material file does not publish it", "[resource]")
{
    ResourceManager manager;
    manager.preload({}, {"no_such_material.ini"});

    REQUIRE(manager.loadMaterial("no_such_material.ini") == invalidResource);
}

TEST_CASE("a material without a texture slot reports an invalid id for it", "[resource]")
{
    TempIni material("[custom]\nkey=value\n");

    ResourceManager manager;
    const ResourceId materialId = manager.loadMaterial(material.path());
    REQUIRE(materialId != invalidResource);

    ResourceId diffuse = 0;
    ResourceId normal = 0;
    ResourceId specular = 0;
    ResourceId glow = 0;
    ResourceId alpha = 0;
    ResourceId customShader = 0;
    REQUIRE(manager.getMaterial(materialId, diffuse, normal, specular, glow, alpha, customShader));

    // An absent slot must not resolve to a valid resource id
    REQUIRE(diffuse == invalidResource);
    REQUIRE(normal == invalidResource);
    REQUIRE(specular == invalidResource);
    REQUIRE(glow == invalidResource);
    REQUIRE(alpha == invalidResource);
}
