#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <list>

#include "command/IConsole.h"

/**
 * \brief Console class.
 * A console can receive and distribute commands to attached command handlers
 * and store console variables.
 */
class CConsole : public IConsole
{
   public:
    void sendCommand(std::string command);

    /**
     * \brief Adds handler for the specified command.
     */
    void addCommandHandler(const std::string& command, ICommandHandler* handler);

    /**
     * \brief Removes handler for the specified command.
     */
    void removeCommandHandler(const std::string& command, ICommandHandler* handler);

   protected:
    void sendCommand(const std::string& command, const std::vector<std::string>& arguments);

    void setVariable(const std::string& name, const std::string& value);
    void unsetVariable(const std::string& name);

   private:
    std::unordered_map<std::string, std::string> m_variables; /**< Stores console variables. */
    std::unordered_map<std::string, std::list<ICommandHandler*>>
        m_commandHandlers; /**< Maps commands to handlers. */
};
