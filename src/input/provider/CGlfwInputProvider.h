#pragma once

#include <map>
#include <set>

#include "input/IInputProvider.h"

struct GLFWwindow;

/**
 * \brief Provides user input.
 * Uses the glfw api.
 */
class CGlfwInputProvider : public IInputProvider
{
   public:
    static CGlfwInputProvider& createInstance(GLFWwindow* window);

    ~CGlfwInputProvider();

    virtual void addInputListener(IInputListener* listener) override;
    virtual void removeInputListener(IInputListener* listener) override;

    virtual bool isKeyPressed(int keyCode) override;

   private:
    CGlfwInputProvider(GLFWwindow* window);

    // TODO this and the static methods should be extracted into their own class
    friend void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void glfwCursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    friend void glfwCursorEnterCallback(GLFWwindow* window, int entered);
    friend void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    friend void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // please don't hate me for this ...
    template <class _T1, class _T2>
    template <class... _Args1, class... _Args2, size_t... _I1, size_t... _I2>
    friend std::pair<_T1, _T2>::pair(std::piecewise_construct_t,
                                     std::tuple<_Args1...>& __first_args,
                                     std::tuple<_Args2...>& __second_args, __tuple_indices<_I1...>,
                                     std::__tuple_indices<_I2...>);

    GLFWwindow* m_window;
    std::set<IInputListener*> m_listeners;
};
