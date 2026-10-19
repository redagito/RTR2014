#pragma once

class AnimationWorld;
class CameraController;
class DebugInfo;
class IDebugInfoDisplay;
class IControllableCamera;
class IGraphicsResourceManager;
class IInputProvider;
class IRenderer;
class IScene;
class IWindow;

/**
* \brief Platform independent demo logic.
*
* Handles the per frame updates: key handling, renderer selection, camera
* sequence playback, animation and the debug info display. Depends only on
* interfaces, so it can run without a graphics context.
*/
class DemoController
{
   public:
    DemoController(IRenderer* deferredRenderer, IRenderer* forwardRenderer, IScene* scene,
                   IControllableCamera* camera, CameraController* cameraController,
                   IInputProvider* inputProvider, IWindow* window,
                   IGraphicsResourceManager* graphicsResourceManager, DebugInfo* debugInfo,
                   IDebugInfoDisplay* debugInfoDisplay, AnimationWorld* animationWorld);

    /**
    * \brief Performs the per frame update.
    */
    void update(float timeDiff);

   private:
    /**
    * \brief Handles the key input and updates the frame counters.
    */
    void updateInput(float timeDiff);

    /**
    * \brief Updates the debug info display.
    */
    void updateDebugInfo(float timeDiff);

    IRenderer* m_renderer = nullptr;
    IRenderer* m_deferredRenderer = nullptr;
    IRenderer* m_forwardRenderer = nullptr;
    IScene* m_scene = nullptr;
    IControllableCamera* m_camera = nullptr;
    CameraController* m_cameraController = nullptr;
    IInputProvider* m_inputProvider = nullptr;
    IWindow* m_window = nullptr;
    IGraphicsResourceManager* m_graphicsResourceManager = nullptr;
    DebugInfo* m_debugInfo = nullptr;
    IDebugInfoDisplay* m_debugInfoDisplay = nullptr;
    AnimationWorld* m_animationWorld = nullptr;

    bool m_displayDebugInfo = false;
    bool m_vSync = true;

    double m_f1Cooldown = 0.0;
    double m_f2Cooldown = 0.0;
    double m_f3Cooldown = 0.0;
    double m_f4Cooldown = 0.0;
    double m_f5Cooldown = 0.0;
    double m_k1Cooldown = 0.0;

    double m_fpsCoolDown = 1.0;
    unsigned int m_lastFrameCount = 0;
    unsigned int m_currentFrameCount = 0;
};