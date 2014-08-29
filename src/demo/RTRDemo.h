#pragma once

#include "command/console/CConsole.h"
#include "command/handler/CPrintHandler.h"

class GLFWwindow;

class RTRDemo
{
   public:
    RTRDemo();
    ~RTRDemo();

    int run();

   private:
    CPrintHandler m_printHandler;
    CConsole m_console;
    GLFWwindow* m_window;
};
