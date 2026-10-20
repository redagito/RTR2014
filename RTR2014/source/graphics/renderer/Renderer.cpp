#include "graphics/renderer/Renderer.h"

#include <cassert>
#include <string>

#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/core/RendererCoreConfig.h"

Renderer::Renderer() { return; }

Renderer::~Renderer() { return; }

void Renderer::draw(const Mesh* mesh)
{
    mesh->getVertexArray()->setActive();

    // Set primitive draw mode
    GLenum mode = Mesh::toGLPrimitive(mesh->getPrimitiveType());
    unsigned int primitiveSize = Mesh::getPrimitiveSize(mesh->getPrimitiveType());
    if (primitiveSize == 0)
    {
        spdlog::error("A mesh with an unknown primitive type cannot be drawn.");
        mesh->getVertexArray()->setInactive();
        return;
    }

    // Decide on draw method based on the stored data
    if (mesh->hasIndexBuffer())
    {
        // Indexed draw, faster
        glDrawElements(mode, mesh->getIndexBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        // Slowest draw method
        glDrawArrays(mode, 0, mesh->getVertexBuffer()->getSize() / primitiveSize);
    }
    mesh->getVertexArray()->setInactive();
}

void Renderer::bindMaterial(ShaderProgram* shader, const Material* material,
                            const glm::mat4& translation, const glm::mat4& rotation,
                            const glm::mat4& scale, const IGraphicsResourceManager& manager)
{
    // Transformation matrices
    shader->setUniform(translationMatrixUniformName, translation);
    shader->setUniform(rotationMatrixUniformName, rotation);
    shader->setUniform(scaleMatrixUniformName, scale);
    shader->setUniform(modelMatrixUniformName, translation * rotation * scale);

    // Send material textures to shader
    if (material->hasDiffuse())
    {
        material->getDiffuse()->setActive(diffuseTextureUnit);
    }
    else
    {
        manager.getDefaultDiffuseTexture()->setActive(diffuseTextureUnit);
    }
    shader->setUniform(diffuseTextureUniformName, diffuseTextureUnit);

    if (material->hasNormal())
    {
        material->getNormal()->setActive(normalTextureUnit);
    }
    else
    {
        manager.getDefaultNormalTexture()->setActive(normalTextureUnit);
    }
    shader->setUniform(normalTextureUniformName, normalTextureUnit);

    if (material->hasSpecular())
    {
        material->getSpecular()->setActive(specularTextureUnit);
    }
    else
    {
        manager.getDefaultSpecularTexture()->setActive(specularTextureUnit);
    }
    shader->setUniform(specularTextureUniformName, specularTextureUnit);

    if (material->hasGlow())
    {
        material->getGlow()->setActive(glowTextureUnit);
    }
    else
    {
        manager.getDefaultGlowTexture()->setActive(glowTextureUnit);
    }
    shader->setUniform(glowTextureUniformName, glowTextureUnit);

    // TODO Sort materials with alpha texture for blending.
    if (material->hasAlpha())
    {
        material->getAlpha()->setActive(alphaTextureUnit);
    }
    else
    {
        manager.getDefaultAlphaTexture()->setActive(alphaTextureUnit);
    }
    shader->setUniform(alphaTextureUniformName, alphaTextureUnit);
}
