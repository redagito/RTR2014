#pragma once

/**
 * \brief Window listener interface class.
 *
 * Receives window events.
 */
class IWindowListener
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IWindowListener();

    /**
     * \brief Handles a window resize event.
     */
    virtual void handleResizeEvent(int width, int height) = 0;
};

/**
 * \brief Window interface class.
 * Provides window dimensions, event polling and the active context.
 */
class IWindow
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IWindow();

    /**
     * \brief Returns current window width.
     */
    virtual unsigned int getWidth() const = 0;

    /**
     * \brief Returns current window height.
     */
    virtual unsigned int getHeight() const = 0;

    /**
     * \brief Sets window active for rendering.
     * Must set the context as current.
     * TODO Should this be const or provide renderer with non const reference?
     */
    virtual void setActive() const = 0;

    /**
     * \brief Returns whether the window is still open.
     */
    virtual bool isOpen() const = 0;

    /**
     * \brief Swaps the front and back buffers.
     */
    virtual void swapBuffer() = 0;

    /**
     * \brief Enables or disables the synchronization with the display refresh.
     */
    virtual void setVSync(bool enabled) = 0;

    /**
     * \brief Toggles mouse capture state.
     */
    virtual void toggleMouseCapture() = 0;

    /**
     * \brief Returns whether the mouse is captured.
     */
    virtual bool isMouseCaptured() const = 0;

    /**
     * \brief Processes pending window events.
     */
    virtual void pollEvents() = 0;

    /**
     * \brief Returns the current time in seconds.
     */
    virtual double getTime() const = 0;

    /**
     * \brief Adds a window event listener.
     */
    virtual void addListener(IWindowListener* listener) = 0;

    /**
     * \brief Removes a window event listener.
     */
    virtual void removeListener(IWindowListener* listener) = 0;
};