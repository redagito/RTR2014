#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <glm/glm.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "resource/IResourceListener.h"
#include "resource/OrientedBoundingBox.h"
#include "resource/ResourceConfig.h"
#include "resource/ResourceManager.h"
#include "resource/SoundData.h"

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

class TempSound
{
   public:
    TempSound()
    {
        std::ifstream ifs(std::string(RTR_TEST_DATA_DIR) + "/tone.mp3", std::ios::binary);
        std::ofstream ofs(m_path, std::ios::binary);
        ofs << ifs.rdbuf();
    }
    ~TempSound() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_sound.mp3";
};

class TempFile
{
   public:
    TempFile(const std::string& name, const std::string& content)
    {
        m_path = name;
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempFile() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path;
};

class SoundListener : public IResourceListener
{
   public:
    void onAttach(IResourceManager*) override {}
    void onDetach(IResourceManager*) override {}
    void notify(ResourceType type, ResourceId id, ListenerEvent event, IResourceManager*) override
    {
        m_events.emplace_back(type, id, event);
    }

    std::vector<std::tuple<ResourceType, ResourceId, ListenerEvent>> m_events;
};

// Removes itself while it is notified.
class SelfRemovingListener : public IResourceListener
{
   public:
    void onAttach(IResourceManager*) override {}
    void onDetach(IResourceManager*) override {}
    void notify(ResourceType, ResourceId, ListenerEvent, IResourceManager* manager) override
    {
        manager->removeResourceListener(this);
    }
};

class CountingListener : public IResourceListener
{
   public:
    void onAttach(IResourceManager*) override {}
    void onDetach(IResourceManager*) override {}
    void notify(ResourceType, ResourceId, ListenerEvent, IResourceManager*) override { ++count; }

    int count = 0;
};

// Records the file of a shader program while the listener is notified, which
// is when the graphics manager reports a compilation error.
class ShaderFileListener : public IResourceListener
{
   public:
    void onAttach(IResourceManager*) override {}
    void onDetach(IResourceManager*) override {}
    void notify(ResourceType type, ResourceId id, ListenerEvent event,
                IResourceManager* manager) override
    {
        if (type != ResourceType::Shader || event != ListenerEvent::Create)
        {
            return;
        }
        std::string file;
        if (manager->getShaderFile(id, file))
        {
            m_file = file;
        }
    }

    std::string m_file;
};

// Loads a mesh again while the listener is notified, which happens inside the
// creation of the resource.
class ReentrantMeshListener : public IResourceListener
{
   public:
    void onAttach(IResourceManager*) override {}
    void onDetach(IResourceManager*) override {}
    void notify(ResourceType type, ResourceId id, ListenerEvent event,
                IResourceManager* manager) override
    {
        if (type != ResourceType::Mesh || event != ListenerEvent::Create || m_reentered)
        {
            return;
        }
        m_reentered = true;
        m_secondId = manager->loadMesh(m_file);
    }

    ResourceId m_secondId = invalidResource;
    std::string m_file;

   private:
    bool m_reentered = false;
};

// The same object loaded with and without preloading must yield the same data.
const char* kObj =
    "v 0 0 0\n"
    "v 1 0 0\n"
    "v 0 1 0\n"
    "vn 0 0 1\n"
    "vt 0 0\n"
    "f 1/1/1 2/1/1 3/1/1\n";

bool contains(const OrientedBoundingBox& box, const glm::vec3& point)
{
    glm::vec3 delta = point - box.getCenter();
    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = glm::vec3(box.getAxes()[i]);
        if (std::fabs(glm::dot(delta, axis)) > box.getHalfExtents()[i] + 0.0001f)
        {
            return false;
        }
    }
    return true;
}
}  // namespace

TEST_CASE("loadImage rejects a file that is not a png", "[resource]")
{
    ResourceManager manager;

    REQUIRE(manager.loadImage("not_an_image.txt", ColorFormat::RGBA32, ColorSpace::Linear) ==
            invalidResource);
}

TEST_CASE("loadImage rejects an unknown color format", "[resource]")
{
    ResourceManager manager;

    REQUIRE(manager.loadImage("image.png", ColorFormat::Invalid, ColorSpace::Linear) ==
            invalidResource);
}

TEST_CASE("loadImage keeps the color space of the image", "[resource]")
{
    ResourceManager manager;
    const ResourceId id = manager.loadImage(RTR_PROJECT_DATA_DIR "/images/brick_1_diffuse.png",
                                            ColorFormat::RGB24, ColorSpace::sRGB);

    REQUIRE(id != invalidResource);
    std::vector<unsigned char> data;
    unsigned int width = 0;
    unsigned int height = 0;
    ColorFormat format = ColorFormat::Invalid;
    ColorSpace colorSpace = ColorSpace::Linear;
    REQUIRE(manager.getImage(id, data, width, height, format, colorSpace));
    REQUIRE(format == ColorFormat::RGB24);
    REQUIRE(colorSpace == ColorSpace::sRGB);
}

TEST_CASE("the same image file with a different color space is a different image", "[resource]")
{
    ResourceManager manager;
    const ResourceId linear = manager.loadImage(RTR_PROJECT_DATA_DIR "/images/brick_1_diffuse.png",
                                                ColorFormat::RGB24, ColorSpace::Linear);
    const ResourceId srgb = manager.loadImage(RTR_PROJECT_DATA_DIR "/images/brick_1_diffuse.png",
                                              ColorFormat::RGB24, ColorSpace::sRGB);

    REQUIRE(linear != invalidResource);
    REQUIRE(srgb != invalidResource);
    REQUIRE(linear != srgb);
}

TEST_CASE("preloaded mesh data matches the synchronously loaded data", "[resource]")
{
    TempObj file(kObj);

    ResourceManager preloaded;
    preloaded.preload({file.path()}, {}, {});
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
    manager.preload({"no_such_file.obj"}, {}, {});

    REQUIRE(manager.loadMesh("no_such_file.obj") == invalidResource);
}

TEST_CASE("preloading an unknown material file does not publish it", "[resource]")
{
    ResourceManager manager;
    manager.preload({}, {"no_such_material.ini"}, {});

    REQUIRE(manager.loadMaterial("no_such_material.ini") == invalidResource);
}

TEST_CASE("a loaded shader program reports the file it was loaded from", "[resource]")
{
    TempFile vertexSource("rtr2014_tmp_vertex.glsl", "void main() {}\n");
    TempFile fragmentSource("rtr2014_tmp_fragment.glsl", "void main() {}\n");
    TempIni shaderProgram("[vertex]\nfile=" + vertexSource.path() +
                          "\n[fragment]\nfile=" + fragmentSource.path() + "\n");

    ResourceManager manager;
    const ResourceId id = manager.loadShader(shaderProgram.path());
    REQUIRE(id != invalidResource);

    std::string file;
    REQUIRE(manager.getShaderFile(id, file));
    REQUIRE(file == shaderProgram.path());
}

TEST_CASE("an unknown shader program has no file", "[resource]")
{
    ResourceManager manager;

    std::string file;
    REQUIRE_FALSE(manager.getShaderFile(invalidResource, file));
}

TEST_CASE("the shader program file is known when the listeners are notified", "[resource]")
{
    TempFile vertexSource("rtr2014_tmp_vertex.glsl", "void main() {}\n");
    TempFile fragmentSource("rtr2014_tmp_fragment.glsl", "void main() {}\n");
    TempIni shaderProgram("[vertex]\nfile=" + vertexSource.path() +
                          "\n[fragment]\nfile=" + fragmentSource.path() + "\n");

    ResourceManager manager;
    ShaderFileListener listener;
    manager.addResourceListener(&listener);

    const ResourceId id = manager.loadShader(shaderProgram.path());
    REQUIRE(id != invalidResource);
    REQUIRE(listener.m_file == shaderProgram.path());
}

TEST_CASE("loading a preloaded mesh from a listener does not publish an empty mesh", "[resource]")
{
    TempObj file(kObj);

    ResourceManager manager;
    manager.preload({file.path()}, {}, {});

    ReentrantMeshListener listener;
    listener.m_file = file.path();
    manager.addResourceListener(&listener);

    const ResourceId id = manager.loadMesh(file.path());
    REQUIRE(id != invalidResource);

    // The listener loaded the same file again, it must not have got a mesh
    // without any data.
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    PrimitiveType type = PrimitiveType::Invalid;
    REQUIRE(manager.getMesh(listener.m_secondId, vertices, indices, normals, uvs, type));
    REQUIRE(vertices.size() == 9);
}

TEST_CASE("a listener can remove itself while it is notified", "[resource]")
{
    ResourceManager manager;
    SelfRemovingListener remover;
    CountingListener counter;
    manager.addResourceListener(&remover);
    manager.addResourceListener(&counter);

    manager.createMesh({}, {}, {}, {}, PrimitiveType::Triangle);

    // The listener behind the one that removed itself is still notified.
    REQUIRE(counter.count == 1);
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

TEST_CASE("a mesh reports bounds that contain its vertices", "[resource]")
{
    TempObj file(kObj);

    ResourceManager manager;
    const ResourceId id = manager.loadMesh(file.path());

    OrientedBoundingBox bounds;
    REQUIRE(manager.getMeshBounds(id, bounds));

    REQUIRE(contains(bounds, glm::vec3(0.f, 0.f, 0.f)));
    REQUIRE(contains(bounds, glm::vec3(1.f, 0.f, 0.f)));
    REQUIRE(contains(bounds, glm::vec3(0.f, 1.f, 0.f)));
}

TEST_CASE("a preloaded mesh reports the same bounds as a synchronously loaded mesh", "[resource]")
{
    TempObj file(kObj);

    ResourceManager preloaded;
    preloaded.preload({file.path()}, {}, {});
    const ResourceId preloadedId = preloaded.loadMesh(file.path());

    ResourceManager synchronous;
    const ResourceId synchronousId = synchronous.loadMesh(file.path());

    OrientedBoundingBox preloadedBounds;
    OrientedBoundingBox synchronousBounds;
    REQUIRE(preloaded.getMeshBounds(preloadedId, preloadedBounds));
    REQUIRE(synchronous.getMeshBounds(synchronousId, synchronousBounds));

    REQUIRE(preloadedBounds.getCenter() == synchronousBounds.getCenter());
    REQUIRE(preloadedBounds.getHalfExtents() == synchronousBounds.getHalfExtents());
}

TEST_CASE("loadSound decodes an mp3 file into pcm samples", "[resource]")
{
    ResourceManager manager;
    const ResourceId id = manager.loadSound(RTR_TEST_DATA_DIR "/tone.mp3");

    REQUIRE(id != invalidResource);
    const SoundData* sound = manager.getSound(id);
    REQUIRE(sound != nullptr);
    REQUIRE(sound->m_sampleRate == 44100);
    REQUIRE(sound->m_channels == 2);
    REQUIRE(sound->m_samples.size() >= sound->m_channels);
    REQUIRE(std::any_of(sound->m_samples.begin(), sound->m_samples.end(),
                        [](int16_t sample) { return sample != 0; }));
}

TEST_CASE("loading the same sound file twice returns the same id", "[resource]")
{
    ResourceManager manager;
    const ResourceId first = manager.loadSound(RTR_TEST_DATA_DIR "/tone.mp3");
    const ResourceId second = manager.loadSound(RTR_TEST_DATA_DIR "/tone.mp3");

    REQUIRE(first != invalidResource);
    REQUIRE(second == first);
}

TEST_CASE("loadSound rejects a missing file", "[resource]")
{
    ResourceManager manager;

    REQUIRE(manager.loadSound("no_such_sound.mp3") == invalidResource);
}

TEST_CASE("loadSound rejects a file that is not audio", "[resource]")
{
    TempObj file("not audio");

    ResourceManager manager;

    REQUIRE(manager.loadSound(file.path()) == invalidResource);
}

TEST_CASE("a preloaded sound is decoded before the first load", "[resource]")
{
    TempSound file;
    ResourceManager manager;
    manager.preload({}, {}, {file.path()});
    std::remove(file.path().c_str());

    const ResourceId id = manager.loadSound(file.path());
    REQUIRE(id != invalidResource);
    const SoundData* sound = manager.getSound(id);
    REQUIRE(sound != nullptr);
    REQUIRE_FALSE(sound->m_samples.empty());
}

TEST_CASE("loading a sound notifies listeners exactly once", "[resource]")
{
    ResourceManager manager;
    SoundListener listener;
    manager.addResourceListener(&listener);

    const ResourceId id = manager.loadSound(RTR_TEST_DATA_DIR "/tone.mp3");
    REQUIRE(id != invalidResource);
    REQUIRE(listener.m_events.size() == 1);
    REQUIRE(std::get<0>(listener.m_events[0]) == ResourceType::Sound);
    REQUIRE(std::get<1>(listener.m_events[0]) == id);
    REQUIRE(std::get<2>(listener.m_events[0]) == ListenerEvent::Create);

    REQUIRE(manager.loadSound(RTR_TEST_DATA_DIR "/tone.mp3") == id);
    REQUIRE(listener.m_events.size() == 1);
}