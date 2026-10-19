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
    Material(const Texture* diffuse, const Texture* normal, const Texture* specular,
              const Texture* glow, const Texture* alpha, ShaderProgram* customShader);

    bool init(const Texture* diffuse, const Texture* normal, const Texture* specular,
              const Texture* glow, const Texture* alpha, ShaderProgram* customShader);

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
    const Texture* m_diffuseTexture;  /**< Base color. */
    const Texture* m_normalTexture;   /**< Normal map. */
    const Texture* m_specularTexture; /**< Specular highlights. */
    const Texture* m_glowTexture;     /**< Glow map. */
    const Texture* m_alphaTexture;    /**< Alpha map. */
    ShaderProgram* m_customShader;    /**< Custom shader. */
};
