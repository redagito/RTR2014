#pragma once

class DebugInfo;

/**
 * \brief Debug info display interface.
 *
 * Renders the collected debug information to the screen.
 */
class IDebugInfoDisplay
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IDebugInfoDisplay();

    /**
     * \brief Draws the debug information.
     */
    virtual void draw(const DebugInfo& info) = 0;
};