#include "CPrintHandler.h"

#include <iostream>

void CPrintHandler::handleCommand(const std::string& command) { return; }

void CPrintHandler::handleCommand(const std::string& command,
                                  const std::vector<std::string>& arguments)
{
    if (command == "print" && arguments.size() == 1)
    {
        std::cout << "[PRINT] " << arguments.at(0) << std::endl;
    }
}
