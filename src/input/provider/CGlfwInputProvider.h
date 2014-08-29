#pragma once

struct GLFWwindow;

/**
 * \brief Provides user input.
 * Uses the glfw api.
 */
class CGlfwInputProvider
{
public:
	CGlfwInputProvider(GLFWwindow* window);

private:
	GLFWwindow* m_window;
};
