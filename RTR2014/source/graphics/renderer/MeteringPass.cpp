#include "graphics/renderer/MeteringPass.h"

#include <spdlog/spdlog.h>

#include <cmath>
#include <string>

#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IWindow.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"

namespace
{
// The size of the texture that holds the measurement. Every texel of it
// stands for a block of the frame.
constexpr unsigned int kMeteringSize = 64;

// Time constant for the exposure history, it is applied with the frame time so
// that the speed does not depend on the frame rate. The exposure settles in a
// few seconds.
constexpr float kAdaptationTimeConstant = 1.2f;
}  // namespace

MeteringPass::MeteringPass() { return; }

MeteringPass::~MeteringPass() { return; }

void MeteringPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                          const std::shared_ptr<Texture>& texture, float deltaTime)
{
    // Get metering shader
    ShaderProgram* meteringShader = manager.getShaderProgram(m_meteringPassShaderId);
    if (meteringShader == nullptr)
    {
        fatalError("Shader program for metering pass could not be retrieved.");
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        fatalError("Mesh object for metering pass could not be retrieved.");
    }

    // The measurement is written into a texture of its own
    m_measurementBuffer.setActive(GL_FRAMEBUFFER);
    glViewport(0, 0, (GLsizei)kMeteringSize, (GLsizei)kMeteringSize);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    meteringShader->setActive();

    texture->setActive(meteringPassSceneTextureUnit);
    meteringShader->setUniform(sceneTextureUniformName, meteringPassSceneTextureUnit);

    // The size of the texture that holds the measurement
    meteringShader->setUniform(meteringWidthUniformName, (float)kMeteringSize);
    meteringShader->setUniform(meteringHeightUniformName, (float)kMeteringSize);

    Renderer::draw(quadMesh);

    m_measurementBuffer.setInactive(GL_FRAMEBUFFER);

    // The measurement is averaged over the whole frame by the mipmap chain of
    // the texture, the top level of it holds the average of the logarithm.
    glGenerateTextureMipmap(m_measurementTexture->getId());

    // Blend the new measurement into the exposure history. The other texture
    // holds the previous history, so the two are swapped after the draw.
    const int writeIndex = 1 - m_currentExposureIndex;
    m_adaptationBuffer.attach(m_adaptedExposureTextures[writeIndex], GL_COLOR_ATTACHMENT0);
    m_adaptationBuffer.setActive(GL_FRAMEBUFFER);
    glViewport(0, 0, 1, 1);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderProgram* adaptationShader = manager.getShaderProgram(m_exposureAdaptationShaderId);
    if (adaptationShader == nullptr)
    {
        fatalError("Shader program for exposure adaptation pass could not be retrieved.");
    }
    adaptationShader->setActive();

    m_measurementTexture->setActive(exposureAdaptationCurrentTextureUnit);
    adaptationShader->setUniform(currentMeasurementUniformName,
                                 exposureAdaptationCurrentTextureUnit);
    m_adaptedExposureTextures[m_currentExposureIndex]->setActive(
        exposureAdaptationPreviousTextureUnit);
    adaptationShader->setUniform(previousExposureUniformName,
                                 exposureAdaptationPreviousTextureUnit);
    const float adaptationFactor =
        m_hasAdaptedExposure ? 1.0f - std::exp(-deltaTime / kAdaptationTimeConstant) : 1.0f;
    adaptationShader->setUniform(adaptationFactorUniformName, adaptationFactor);

    Renderer::draw(quadMesh);

    m_adaptationBuffer.setInactive(GL_FRAMEBUFFER);
    m_currentExposureIndex = writeIndex;
    m_hasAdaptedExposure = true;
}

bool MeteringPass::init(IResourceManager* manager)
{
    // Metering shader
    std::string meteringShaderFile = "data/shader/post/metering_pass.ini";
    m_meteringPassShaderId = manager->loadShader(meteringShaderFile);
    // Check if ok
    if (m_meteringPassShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.", meteringShaderFile.c_str());
    }

    // Exposure adaptation shader
    std::string adaptationShaderFile = "data/shader/post/exposure_adaptation_pass.ini";
    m_exposureAdaptationShaderId = manager->loadShader(adaptationShaderFile);
    // Check if ok
    if (m_exposureAdaptationShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.", adaptationShaderFile.c_str());
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_screenQuadId = manager->loadMesh(quadMesh);
    // Check if ok
    if (m_screenQuadId == invalidResource)
    {
        fatalError("Failed to load screen quad mesh {}.", quadMesh.c_str());
    }

    // The texture that holds the measurement, the mipmap chain of it holds
    // the averages of the frame.
    m_measurementTexture = std::make_shared<Texture>();
    if (!m_measurementTexture->init(kMeteringSize, kMeteringSize, GL_R16F, true))
    {
        fatalError("Failed to initialize the measurement texture.");
    }
    m_measurementBuffer.attach(m_measurementTexture, GL_COLOR_ATTACHMENT0);
    m_measurementBuffer.setInactive(GL_FRAMEBUFFER);
    spdlog::info("Metering buffer state: {}.", m_measurementBuffer.getState().c_str());

    // The textures hold the exposure history, only one texel is needed.
    for (auto& texture : m_adaptedExposureTextures)
    {
        texture = std::make_shared<Texture>();
        if (!texture->init(1, 1, GL_R16F))
        {
            fatalError("Failed to initialize the exposure history texture.");
        }
    }
    m_adaptationBuffer.attach(m_adaptedExposureTextures[0], GL_COLOR_ATTACHMENT0);
    m_adaptationBuffer.setInactive(GL_FRAMEBUFFER);
    spdlog::info("Exposure adaptation buffer state: {}.", m_adaptationBuffer.getState().c_str());

    return true;
}

const std::shared_ptr<Texture>& MeteringPass::getMeasurementTexture() const
{
    return m_adaptedExposureTextures[m_currentExposureIndex];
}
