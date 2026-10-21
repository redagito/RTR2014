#include "graphics/renderer/GaussBlurPass.h"

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

GaussBlurPass::GaussBlurPass() { return; }

GaussBlurPass::~GaussBlurPass() { return; }

void GaussBlurPass::renderVertical(const IWindow& window, const IGraphicsResourceManager& manager,
                                   const std::shared_ptr<Texture>& texture)
{
    // Get gauss shader
    ShaderProgram* shader = manager.getShaderProgram(m_gaussBlurVerticalShaderId);
    if (shader == nullptr)
    {
        fatalError("Shader program for vertical gauss blur pass could not be retrieved.");
    }
    shader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        fatalError("Mesh object for vertical gauss blur pass could not be retrieved.");
    }

    // Input texture
    texture->setActive(gaussBlurVerticalPassInputTextureUnit);
    shader->setUniform(texture0UniformName, gaussBlurVerticalPassInputTextureUnit);

    // Blur parameter
    shader->setUniform(blurStrengthUniformName, 3.f);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    Renderer::draw(quadMesh);
}

void GaussBlurPass::renderHorizontal(const IWindow& window, const IGraphicsResourceManager& manager,
                                     const std::shared_ptr<Texture>& texture)
{
    // Get gauss shader
    ShaderProgram* shader = manager.getShaderProgram(m_gaussBlurHorizontalShaderId);
    if (shader == nullptr)
    {
        fatalError("Shader program for horizontal gauss blur pass could not be retrieved.");
    }
    shader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        fatalError("Mesh object for horizontal gauss blur pass could not be retrieved.");
    }

    // Input texture
    texture->setActive(gaussBlurHoriontalPassInputTextureUnit);
    shader->setUniform(texture0UniformName, gaussBlurHoriontalPassInputTextureUnit);

    // Blur parameter
    shader->setUniform(blurStrengthUniformName, 3.f);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    Renderer::draw(quadMesh);
}

bool GaussBlurPass::init(IResourceManager* manager)
{
    // Vertical gauss blur shader
    std::string gaussBlurVerticalShaderFile = "data/shader/post/gauss_blur_vertical_pass.ini";
    m_gaussBlurVerticalShaderId = manager->loadShader(gaussBlurVerticalShaderFile);
    // Check if ok
    if (m_gaussBlurVerticalShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.",
                   gaussBlurVerticalShaderFile.c_str());
    }

    // Horizontal gauss blur shader
    std::string gaussBlurHorizontalShaderFile = "data/shader/post/gauss_blur_horizontal_pass.ini";
    m_gaussBlurHorizontalShaderId = manager->loadShader(gaussBlurHorizontalShaderFile);
    // Check if ok
    if (m_gaussBlurHorizontalShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.",
                   gaussBlurHorizontalShaderFile.c_str());
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