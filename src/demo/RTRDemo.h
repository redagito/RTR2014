#pragma once

#ifdef _WIN32
#    include "generated/flextGL.h"
#else
#    define GLFW_INCLUDE_GLCOREARB
#    define flextInit
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
