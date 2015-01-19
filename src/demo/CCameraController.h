#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "graphics/window/CGlfwWindow.h"

#include "input/IInputListener.h"

class IControllableCamera;
class IInputProvider;

class CCameraController : public IInputListener, public IGlfwWindowListener
{
   public:
    CCameraController();
    ~CCameraController();

    void setCamera(std::shared_ptr<IControllableCamera> camera);
    void setInputProvider(IInputProvider* provider);

    bool loadSequence(std::string file);
    
    void animate(float dt);

    virtual void handleKeyEvent(EKeyEventType type, int keyCode) override;
    virtual void handleMouseMovementEvent(int x, int y) override;
    virtual void handleMouseButtonEvent(EMouseButtonEventType type, int buttonCode) override;
    
    virtual void handleResizeEvent(int width, int height) override;

   private:
    // TODO "animate" is a stupid name
    void animateFeatures(float dt);
    void animateSequence(float dt);
    void animateManual(float dt);
    
    struct SequencePoint {
        glm::vec3 position;
        glm::vec3 orientation;
        float timestamp;
        bool fxaaActive;
        bool fogActive;
    };
    
    bool m_isRunningSequence = false;
    float m_sequenceTime = 0;
    
    std::vector<SequencePoint> m_sequencePoints;
    
    float m_speed = 1.0f;
    std::shared_ptr<IControllableCamera> m_camera = nullptr;
    IInputProvider* m_inputProvider = nullptr;
    int m_lastX = 0;
    int m_lastY = 0;
};