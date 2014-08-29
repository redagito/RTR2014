#include "CGlfwInputProvider.h"

#include <cassert>

#include <GLFW/glfw3.h>

CGlfwInputProvider::CGlfwInputProvider(GLFWwindow* window)
	:
	m_window(window)
{
	assert(m_window != nullptr);
}
