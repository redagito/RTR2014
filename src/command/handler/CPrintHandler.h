#pragma once

#include "command/ICommandHandler.h"

class CPrintHandler : public ICommandHandler
{
   public:
    void handleCommand(const std::vector<std::string>& arguments);
};
