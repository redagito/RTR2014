#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <string>

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
    std::string m_path = "rtr2014_tmp_warn.obj";
};
} // namespace

TEST_CASE("loadMesh reports a warning without failing the load", "[resource]")
{
    // References an mtl that is not present, which tinyobj reports as a warning.
    TempObj file("mtllib does_not_exist.mtl\n"
                 "v 0 0 0\n"
                 "v 1 0 0\n"
                 "v 0 1 0\n"
                 "f 1 2 3\n");

    ResourceManager manager;
    const ResourceId id = manager.loadMesh(file.path());

    REQUIRE(id != invalidResource);
}