#pragma once

#include <memory>
#include <vector>

class IScene;
class IWindow;
class ICamera;

/**
* \brief Renderer interface class.
*/
class IRenderer
{
   public:
    /**
	* \brief Virtual destructor for interface class.
	*/
    virtual ~IRenderer();

    /**
    * \brief Draw scene viewed from camera into window.
    */
    virtual void draw(const IScene& scene, const ICamera& camera, const IWindow& window) = 0;
};