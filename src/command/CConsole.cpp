#include "CConsole.h"

void CConsole::handleCommand(const std::string& command)
{
    auto iter = m_commandHandlers.find(command);
    if (iter == m_commandHandlers.end())
    {
        // No handlers found
        return;
    }

    // Send command to handlers
    for (auto handler : iter->second)
    {
        handler->handleCommand(command);
    }
}

void CConsole::handleCommand(const std::string& command, const std::string& arg0)
{
    std::vector<std::string> args;
    args.push_back(arg0);
    handleCommand(command, args);
}

void CConsole::handleCommand(const std::string& command, const std::string& arg0,
                             const std::string& arg1)
{
    std::vector<std::string> args;
    args.push_back(arg0);
    args.push_back(arg1);
    handleCommand(command, args);
}

void CConsole::handleCommand(const std::string& command, const std::string& arg0,
                             const std::string& arg1, const std::string& arg2)
{
    std::vector<std::string> args;
    args.push_back(arg0);
    args.push_back(arg1);
    args.push_back(arg2);
    handleCommand(command, args);
}

void CConsole::handleCommand(const std::string& command, const std::vector<std::string>& arguments)
{
    // Handle commands for variable/value management
    if (command == "set" && arguments.size() == 2)  // Adds variable to value mapping
    {
        // Arguments not empty
        if (!arguments.at(0).empty() && !arguments.at(1).empty())
        {
            // Set variable
            setVariable(arguments.at(0), arguments.at(1));
        }
    }
    else if (command == "unset" && arguments.size() == 1)  // Removes variable
    {
        if (!arguments.at(0).empty())
        {
            unsetVariable(arguments.at(0));
        }
    }
    else  // Other commands are forwarded
    {
        // Copy argument list
        std::vector<std::string> newArguments = arguments;
        // Traverse arguments and replace variables with their respective values
        for (auto& argument : newArguments)
        {
            // '$' indicates variable
            if (argument.size() > 1 && argument.at(0) == '$')
            {
                // Variable mapping exists
                auto iter = m_variables.find(argument.substr(1));
                if (iter != m_variables.end())
                {
                    // Replace argument with value
                    argument = iter->second;
                }
            }
        }

        // Get handler list
        auto iter = m_commandHandlers.find(command);
        if (iter == m_commandHandlers.end())
        {
            // No handlers found
            return;
        }

        // Forward command to handlers
        for (auto handler : iter->second)
        {
            handler->handleCommand(command, newArguments);
        }
    }
}

void CConsole::addCommandHandler(const std::string& command, ICommandHandler* handler)
{
    m_commandHandlers[command].push_back(handler);
}

void CConsole::removeCommandHandler(const std::string& command, ICommandHandler* handler)
{
    m_commandHandlers[command].remove(handler);
}

void CConsole::setVariable(const std::string& name, const std::string& value)
{
    m_variables[name] = value;
}

void CConsole::unsetVariable(const std::string& name) { m_variables.erase(name); }
