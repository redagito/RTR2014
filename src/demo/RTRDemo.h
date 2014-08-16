#pragma once

#ifndef __APPLE__
#    include "generated/flextGL.h"
#else
#    define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>

class RTRDemo
{
   public:
    RTRDemo();
    ~RTRDemo();

    int run();

   private:
    GLFWwindow* m_window;
};
