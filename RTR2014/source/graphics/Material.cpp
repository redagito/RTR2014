#include <spdlog/spdlog.h>
#include "graphics/Material.h"


Material::Material(const Texture* diffuse, const Texture* normal, const Texture* specular,
                     const Texture* glow, const Texture* alpha, ShaderProgram* customShader)
    : m_diffuseTexture(nullptr),
      m_normalTexture(nullptr),
      m_specularTexture(nullptr),
      m_glowTexture(nullptr),
      m_alphaTexture(nullptr),
      m_customShader(nullptr)
{
    init(diffuse, normal, specular, glow, alpha, customShader);
}

bool Material::init(const Texture* diffuse, const Texture* normal, const Texture* specular,
                     const Texture* glow, const Texture* alpha, ShaderProgram* customShader)
{
    // Textures must either be nullptr (unused) or valid
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
    // Custom shader must either be nullptr for unused or valid
    if (customShader != nullptr && !customShader->isValid())
    {
        return false;
    }

    // All checks ok, set values
    m_diffuseTexture = diffuse;
    m_normalTexture = normal;
    m_specularTexture = specular;
    m_glowTexture = glow;
    m_alphaTexture = alpha;
    m_customShader = customShader;
    return true;
}

bool Material::hasDiffuse() const { return m_diffuseTexture != nullptr; }

bool Material::hasNormal() const { return m_normalTexture != nullptr; }

bool Material::hasSpecular() const { return m_specularTexture != nullptr; }

bool Material::hasGlow() const { return m_glowTexture != nullptr; }

bool Material::hasAlpha() const { return m_alphaTexture != nullptr; }

bool Material::hasCustomShader() const { return m_customShader != nullptr; }

const Texture* Material::getDiffuse() const { return m_diffuseTexture; }

const Texture* Material::getNormal() const { return m_normalTexture; }

const Texture* Material::getSpecular() const { return m_specularTexture; }

const Texture* Material::getGlow() const { return m_glowTexture; }

const Texture* Material::getAlpha() const { return m_alphaTexture; }

ShaderProgram* Material::getCustomShader() const { return m_customShader; }
