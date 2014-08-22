#pragma once

#include <memory>

#include "CTexture.h"

/**
 * \brief Material class.
 *
 * Stores visual properties of a surface.
 */
class CMaterial
{
public:


private:
	std::shared_ptr<CTexture> m_diffuseTexture; /**< Base color. */
	std::shared_ptr<CTexture> m_normalTexture; /**< Normal map for the diffuse texture. */
	std::shared_ptr<CTexture> m_specularTexture; /**< Specular highlights. */
	std::shared_ptr<CTexture> m_glowTexture; /**< Glow map. */
	std::shared_ptr<CTexture> m_alphaTexture; /**< Alpha map. */
};
