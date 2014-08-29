#include "CPrintHandler.h"

#include <iostream>

void CPrintHandler::handleCommand(const std::vector<std::string>& arguments)
{
    std::cout << "[PRINT] ";
    for (const auto& arg : arguments)
    {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
}
