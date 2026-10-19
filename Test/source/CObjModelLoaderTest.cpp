#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <string>

#include "io/CObjModelLoader.h"

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
    std::string m_path = "rtr2014_tmp_test.obj";
};

const char* kValidObj = "v 0 0 0\n"
                        "v 1 0 0\n"
                        "v 0 1 0\n"
                        "vt 0 0\n"
                        "vt 1 0\n"
                        "vt 0 1\n"
                        "vn 0 0 1\n"
                        "vn 0 0 1\n"
                        "vn 0 0 1\n"
                        "f 1/1/1 2/2/2 3/3/3\n";
} // namespace

TEST_CASE("CObjModelLoader loads a well formed obj", "[io]")
{
    TempObj file(kValidObj);
    CObjModelLoader loader;

    REQUIRE(loader.load(file.path()));
    REQUIRE(loader.getVertices().size() == 9);
    REQUIRE(loader.getNormals().size() == 9);
    REQUIRE(loader.getUV().size() == 6);
}

TEST_CASE("CObjModelLoader rejects a face index past the end of the vertex array", "[io]")
{
    TempObj file("v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvn 0 0 1\nf 1/1/1 2/1/1 99/1/1\n");
    CObjModelLoader loader;

    REQUIRE_FALSE(loader.load(file.path()));
}

TEST_CASE("CObjModelLoader rejects a texture index when the obj has no vt section", "[io]")
{
    TempObj file("v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1/1/1 2/1/1 3/1/1\n");
    CObjModelLoader loader;

    REQUIRE_FALSE(loader.load(file.path()));
}

TEST_CASE("CObjModelLoader rejects a negative face index", "[io]")
{
    TempObj file("v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvn 0 0 1\nf -1/1/1 2/1/1 3/1/1\n");
    CObjModelLoader loader;

    REQUIRE_FALSE(loader.load(file.path()));
}

TEST_CASE("CObjModelLoader survives a truncated face line", "[io]")
{
    TempObj file("v 0 0 0\nf\nvt\nvn\n");
    CObjModelLoader loader;

    REQUIRE_FALSE(loader.load(file.path()));
}

TEST_CASE("CObjModelLoader clears previous data between loads", "[io]")
{
    TempObj valid(kValidObj);
    CObjModelLoader loader;

    REQUIRE(loader.load(valid.path()));
    REQUIRE(loader.load(valid.path()));
    REQUIRE(loader.getVertices().size() == 9);
}