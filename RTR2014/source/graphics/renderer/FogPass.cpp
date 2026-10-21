#include "graphics/renderer/FogPass.h"

#include <string>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IWindow.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"

FogPass::FogPass() { return; }

FogPass::~FogPass() { return; }

void FogPass::render(const ICamera& camera, const IWindow& window,
                     const IGraphicsResourceManager& manager,
                     const std::shared_ptr<Texture>& texture,
                     const std::shared_ptr<Texture>& depthTexture)
{
    // Get fog shader
    ShaderProgram* fogShader = manager.getShaderProgram(m_fogPassShaderId);
    if (fogShader == nullptr)
    {
        fatalError("Shader program for fog pass could not be retrieved.");
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        fatalError("Mesh object for fog pass could not be retrieved.");
    }

    fogShader->setActive();

    texture->setActive(fogPassSceneTextureUnit);
    fogShader->setUniform(sceneTextureUniformName, fogPassSceneTextureUnit);

    depthTexture->setActive(fogPassDepthTextureUnit);
    fogShader->setUniform(depthTextureUniformName, fogPassDepthTextureUnit);

    fogShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    fogShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // The depth is linearized with the planes of the camera.
    fogShader->setUniform(cameraZNearUniformName, camera.getZNear());
    fogShader->setUniform(cameraZFarUniformName, camera.getZFar());

    int fogType = 0;
    switch (camera.getFeatureInfo().fogType)
    {
    case FogType::None:
        fogType = 0;
        break;
    case FogType::Linear:
        fogType = 1;
        break;
    case FogType::Exp:
        fogType = 2;
        break;
    case FogType::Exp2:
        fogType = 3;
        break;
    default:
        fogType = 0;
    }
    fogShader->setUniform(fogPassTypeUniformName, fogType);

    Renderer::draw(quadMesh);
}

bool FogPass::init(IResourceManager* manager)
{
    // Fog shader
    std::string fogShaderFile = "data/shader/post/fog_pass.ini";
    m_fogPassShaderId = manager->loadShader(fogShaderFile);
    // Check if ok
    if (m_fogPassShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.", fogShaderFile.c_str());
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_screenQuadId = manager->loadMesh(quadMesh);
    if (m_screenQuadId == invalidResource)
    {
        fatalError("Failed to load screen quad mesh {}.", quadMesh.c_str());
    }

    return true;
}