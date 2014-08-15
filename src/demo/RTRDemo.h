#pragma once

//#define GLFW_INCLUDE_GLCOREARB
#include "generated/flextGL.h"
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
