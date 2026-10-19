#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <string>

#include "animation/AnimationWorld.h"
#include "graphics/scene/Scene.h"
#include "io/SceneLoader.h"
#include "resource/ResourceManager.h"

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

TEST_CASE("SceneLoader rejects a scene object that is not an object node", "[io]")
{
    TempJson file("{\"scene_objects\": [42]}");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a scene whose root is an array", "[io]")
{
    TempJson file("[1, 2, 3]");

    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare(file.path()));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}

TEST_CASE("SceneLoader rejects a missing file", "[io]")
{
    ResourceManager resourceManager;
    AnimationWorld animationWorld;
    Scene scene;

    SceneLoader loader(resourceManager);
    REQUIRE_FALSE(loader.prepare("does_not_exist_scene.json"));
    REQUIRE_FALSE(loader.load(scene, animationWorld));
}