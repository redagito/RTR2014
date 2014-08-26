#pragma once

/**
 * \brief Window interface class.
 * Provides window dimensions and active context.
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
};
