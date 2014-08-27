#pragma once

#include "command/ICommandHandler.h"

class CPrintHandler : public ICommandHandler
{
   public:
    void handleCommand(const std::string& command);
    void handleCommand(const std::string& command, const std::vector<std::string>& arguments);
};
