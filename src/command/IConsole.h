#pragma once

#include <string>

class ICommandHandler;

/**
 * \brief Console interface class.
 * Represents a command distribution hub and handler registry.
 */
class IConsole
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IConsole();

    /**
     * \brief Sends command to console.
     */
    virtual void sendCommand(std::string command) = 0;

    /**
     * \brief Adds handler for the specified command.
     */
    virtual void addCommandHandler(const std::string& command, ICommandHandler* handler) = 0;

    /**
     * \brief Removes handler for the specified command.
     */
    virtual void removeCommandHandler(const std::string& command, ICommandHandler* handler) = 0;
};
