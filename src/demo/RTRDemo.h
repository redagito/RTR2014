#pragma once

class GLFWwindow;

class RTRDemo
{
   public:
    RTRDemo();
    ~RTRDemo();

    int run();

   private:
    GLFWwindow* m_window;
};
