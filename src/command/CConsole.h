#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <list>

#include "ICommandHandler.h"

/**
 * \brief Console class.
 * A console can receive and distribute commands to attached command handlers
 * and store console variables.
 */
class CConsole : public ICommandHandler
{
   public:
    void handleCommand(const std::string& command);
    void handleCommand(const std::string& command, const std::string& arg0);
    void handleCommand(const std::string& command, const std::string& arg0,
                       const std::string& arg1);
    void handleCommand(const std::string& command, const std::string& arg0, const std::string& arg1,
                       const std::string& arg2);
    void handleCommand(const std::string& command, const std::vector<std::string>& arguments);

    /**
     * \brief Adds handler for the specified command.
     */
    void addCommandHandler(const std::string& command, ICommandHandler* handler);

    /**
     * \brief Removes handler for the specified command.
     */
    void removeCommandHandler(const std::string& command, ICommandHandler* handler);

    /**
     * \brief Sets variable to specified value.
     */
    void setVariable(const std::string& name, const std::string& value);

    /**
     * \brief Removes variable mapping.
     */
    void unsetVariable(const std::string& name);

   private:
    std::unordered_map<std::string, std::string> m_variables; /**< Stores console variables. */
    std::unordered_map<std::string, std::list<ICommandHandler*>>
        m_commandHandlers; /**< Maps commands to handlers. */
};
