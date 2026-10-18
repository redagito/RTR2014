#pragma once

enum class EKeyEventType {
    KEY_PRESSED,
    KEY_REPEAT,
    KEY_RELEASED
};

enum class EMouseButtonEventType {
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_RELEASED
};

/**
 * \brief Input handler interface class.
 */
class IInputListener
{
   public:
    virtual ~IInputListener();
    
    virtual void handleKeyEvent(EKeyEventType type, int keyCode) = 0;
    virtual void handleMouseMovementEvent(int x, int y) = 0;
    virtual void handleMouseButtonEvent(EMouseButtonEventType type, int buttonCode) = 0;
};
