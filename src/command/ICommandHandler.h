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
     * \brief Handles command arguments.
     */
    virtual void handleCommand(const std::vector<std::string>& arguments) = 0;
};
