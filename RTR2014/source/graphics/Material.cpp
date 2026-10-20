#include "graphics/Material.h"

#include <spdlog/spdlog.h>

Material::Material(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal,
                   std::shared_ptr<Texture> specular, std::shared_ptr<Texture> glow,
                   std::shared_ptr<Texture> alpha, std::shared_ptr<ShaderProgram> customShader)
    : m_diffuseTexture(nullptr),
      m_normalTexture(nullptr),
      m_specularTexture(nullptr),
      m_glowTexture(nullptr),
      m_alphaTexture(nullptr),
      m_customShader(nullptr)
{
    init(std::move(diffuse), std::move(normal), std::move(specular), std::move(glow),
         std::move(alpha), std::move(customShader));
}

bool Material::init(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal,
                    std::shared_ptr<Texture> specular, std::shared_ptr<Texture> glow,
                    std::shared_ptr<Texture> alpha, std::shared_ptr<ShaderProgram> customShader)
{
    // Textures must either be unset (unused) or valid
    if (diffuse != nullptr && !diffuse->isValid())
    {
        spdlog::error("Diffuse texture is not valid.");
        return false;
    }
    if (normal != nullptr && !normal->isValid())
    {
        spdlog::error("Normal texture is not valid.");
        return false;
    }
    if (specular != nullptr && !specular->isValid())
    {
        spdlog::error("Specular texture is not valid.");
        return false;
    }
    if (glow != nullptr && !glow->isValid())
    {
        spdlog::error("Glow texture is not valid.");
        return false;
    }
    if (alpha != nullptr && !alpha->isValid())
    {
        spdlog::error("Alpha texture is not valid.");
        return false;
    }
    // Custom shader must either be unset for unused or valid
    if (customShader != nullptr && !customShader->isValid())
    {
        return false;
    }

    // All checks ok, set values
    m_diffuseTexture = std::move(diffuse);
    m_normalTexture = std::move(normal);
    m_specularTexture = std::move(specular);
    m_glowTexture = std::move(glow);
    m_alphaTexture = std::move(alpha);
    m_customShader = std::move(customShader);
    return true;
}

bool Material::hasDiffuse() const { return m_diffuseTexture != nullptr; }

bool Material::hasNormal() const { return m_normalTexture != nullptr; }

bool Material::hasSpecular() const { return m_specularTexture != nullptr; }

bool Material::hasGlow() const { return m_glowTexture != nullptr; }

bool Material::hasAlpha() const { return m_alphaTexture != nullptr; }

bool Material::hasCustomShader() const { return m_customShader != nullptr; }

const Texture* Material::getDiffuse() const { return m_diffuseTexture.get(); }

const Texture* Material::getNormal() const { return m_normalTexture.get(); }

const Texture* Material::getSpecular() const { return m_specularTexture.get(); }

const Texture* Material::getGlow() const { return m_glowTexture.get(); }

const Texture* Material::getAlpha() const { return m_alphaTexture.get(); }

ShaderProgram* Material::getCustomShader() const { return m_customShader.get(); }
