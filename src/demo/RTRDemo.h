#pragma once

struct GLFWwindow;

class RTRDemo
{
   public:
    RTRDemo();
    ~RTRDemo();

    int run();

   private:
    GLFWwindow* m_window;
};
