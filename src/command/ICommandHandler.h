#pragma once

#include <string>
#include <vector>

class ICommandHandler
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~ICommandHandler();

    /**
     * \brief handles command without arguments.
     */
    virtual void handleCommand(const std::string& command) = 0;

    /**
     * \brief handles command with arguments.
     */
    virtual void handleCommand(const std::string& command,
                               const std::vector<std::string>& arguments) = 0;
};
