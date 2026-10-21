#include "graphics/renderer/SkyPass.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <vector>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IWindow.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "resource/ResourceConfig.h"

SkyPass::SkyPass() : m_cubemap(std::make_shared<Cubemap>()) {}

SkyPass::~SkyPass() {}

bool SkyPass::init(IResourceManager* manager, const std::string& cubemapFile)
{
    if (cubemapFile.empty())
    {
        // The scene does not have a sky.
        return true;
    }

    const ResourceId imageId = manager->loadImage(cubemapFile, ColorFormat::RGB24);
    if (imageId == invalidResource)
    {
        spdlog::error("Failed to load the sky cubemap file {}.", cubemapFile.c_str());
        return false;
    }

    std::vector<unsigned char> pixels;
    unsigned int width = 0;
    unsigned int height = 0;
    ColorFormat format = ColorFormat::Invalid;
    if (!manager->getImage(imageId, pixels, width, height, format))
    {
        spdlog::error("Failed to read the sky cubemap file {}.", cubemapFile.c_str());
        return false;
    }

    // The atlas is a cross of square faces, four of them in a row and three
    // rows high.
    const unsigned int faceSize = std::min(width / 4, height / 3);
    if (faceSize == 0)
    {
        spdlog::error("The sky cubemap file {} is too small.", cubemapFile.c_str());
        return false;
    }
    if (!m_cubemap->initFromAtlas(pixels, width, height, faceSize))
    {
        spdlog::error("Failed to build the sky cubemap from file {}.", cubemapFile.c_str());
        return false;
    }

    // The shader that draws the sky
    const std::string skyShaderFile = "data/shader/deferred/sky.ini";
    m_skyShaderId = manager->loadShader(skyShaderFile);
    if (m_skyShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", skyShaderFile.c_str());
        return false;
    }

    // The screen quad the sky is drawn with
    const std::string quadMesh = "data/mesh/screen_quad.obj";
    m_screenQuadId = manager->loadMesh(quadMesh);
    if (m_screenQuadId == invalidResource)
    {
        spdlog::error("Failed to load screen quad mesh {}.", quadMesh.c_str());
        return false;
    }

    m_valid = true;
    return true;
}

void SkyPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                     const ICamera& camera)
{
    if (!m_valid)
    {
        return;
    }

    ShaderProgram* skyShader = manager.getShaderProgram(m_skyShaderId);
    if (skyShader == nullptr)
    {
        spdlog::error("Shader program for the sky could not be retrieved.");
        return;
    }
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for the sky could not be retrieved.");
        return;
    }

    // The sky is behind everything, it may neither be hidden by the scene nor
    // hide it.
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    skyShader->setActive();
    m_cubemap->setActive(skyCubemapTextureUnit);
    skyShader->setUniform(skyCubemapUniformName, skyCubemapTextureUnit);
    skyShader->setUniform(cameraPositionUniformName, camera.getPosition());
    skyShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    skyShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // The direction of the sky is the direction of the far plane, the inverse
    // view projection turns it back into the world.
    const glm::mat4 inverseViewProjection = glm::inverse(camera.getProjection() * camera.getView());
    skyShader->setUniform(inverseViewProjectionMatrixUniformName, inverseViewProjection);

    Renderer::draw(quadMesh);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

bool SkyPass::isValid() const { return m_valid; }
