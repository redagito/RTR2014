#pragma once

enum class KeyEventType
{
    KEY_PRESSED,
    KEY_REPEAT,
    KEY_RELEASED
};

enum class MouseButtonEventType
{
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

    virtual void handleKeyEvent(KeyEventType type, int keyCode) = 0;
    virtual void handleMouseMovementEvent(int x, int y) = 0;
    virtual void handleMouseButtonEvent(MouseButtonEventType type, int buttonCode) = 0;
};
