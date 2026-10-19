#pragma once

#include <memory>
#include <unordered_map>

// Required for inheritance
#include "graphics/IRenderer.h"

class IGraphicsResourceManager;
class Mesh;

/**
* \brief Abstract renderer base class.
*/
class Renderer : public IRenderer
{
   public:
    Renderer();

    /**
    * \brief Cleans up resource callbacks.
    */
    virtual ~Renderer();

    /**
    * \brief Draws whole scene.
    *
	* Implemented in specialized rendering classes.
    */
	virtual void draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager) = 0;

   protected:
    /**
    * \brief Performs GL draw call based on mesh data.
	*
    * Sets vertex array object active and performs either indexed or non-indexed draw call.
    * Shader must be set by caller.
    */
    void draw(Mesh* mesh);

};
