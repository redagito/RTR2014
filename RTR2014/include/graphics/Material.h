#pragma once

#include <memory>

#include "Texture.h"
#include "ShaderProgram.h"

/**
 * \brief Material class.
 *
 * Stores visual properties of a surface.
 */
class Material
{
   public:
    Material(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal,
             std::shared_ptr<Texture> specular, std::shared_ptr<Texture> glow,
             std::shared_ptr<Texture> alpha, std::shared_ptr<ShaderProgram> customShader);

    bool init(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal,
              std::shared_ptr<Texture> specular, std::shared_ptr<Texture> glow,
              std::shared_ptr<Texture> alpha, std::shared_ptr<ShaderProgram> customShader);

    bool hasDiffuse() const;
    bool hasNormal() const;
    bool hasSpecular() const;
    bool hasGlow() const;
    bool hasAlpha() const;
    bool hasCustomShader() const;

    const Texture* getDiffuse() const;
    const Texture* getNormal() const;
    const Texture* getSpecular() const;
    const Texture* getGlow() const;
    const Texture* getAlpha() const;
    ShaderProgram* getCustomShader() const;

   private:
    // The textures and the shader are shared with the graphics resource
    // manager, the material keeps them alive.
    std::shared_ptr<Texture> m_diffuseTexture;  /**< Base color. */
    std::shared_ptr<Texture> m_normalTexture;   /**< Normal map. */
    std::shared_ptr<Texture> m_specularTexture; /**< Specular highlights. */
    std::shared_ptr<Texture> m_glowTexture;     /**< Glow map. */
    std::shared_ptr<Texture> m_alphaTexture;    /**< Alpha map. */
    std::shared_ptr<ShaderProgram> m_customShader; /**< Custom shader. */
};
