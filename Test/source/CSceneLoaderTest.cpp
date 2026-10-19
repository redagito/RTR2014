#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <string>

#include "animation/CAnimationWorld.h"
#include "graphics/scene/CScene.h"
#include "io/CSceneLoader.h"
#include "resource/core/CResourceManager.h"

namespace
{
class TempJson
{
   public:
    explicit TempJson(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempJson() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_scene.json";
};
} // namespace

TEST_CASE("CSceneLoader rejects a scene object that is not an object node", "[io]")
{
    TempJson file("{\"scene_objects\": [42]}");

    CResourceManager resourceManager;
    CAnimationWorld animationWorld;
    CScene scene;

    CSceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.load(file.path(), scene, animationWorld));
}

TEST_CASE("CSceneLoader rejects a scene whose root is an array", "[io]")
{
    TempJson file("[1, 2, 3]");

    CResourceManager resourceManager;
    CAnimationWorld animationWorld;
    CScene scene;

    CSceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.load(file.path(), scene, animationWorld));
}

TEST_CASE("CSceneLoader rejects a missing file", "[io]")
{
    CResourceManager resourceManager;
    CAnimationWorld animationWorld;
    CScene scene;

    CSceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.load("does_not_exist_scene.json", scene, animationWorld));
}