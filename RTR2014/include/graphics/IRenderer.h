#pragma once

class IScene;
class IWindow;
class ICamera;
class IGraphicsResourceManager;

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
    virtual void draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager) = 0;
};
