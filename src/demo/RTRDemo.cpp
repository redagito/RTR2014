#include "RTRDemo.h"

#include <string>
#include <vector>

#include "debug/Log.h"

#include "graphics/renderer/core/RendererCoreConfig.h"
#include "graphics/renderer/debug/RendererDebug.h"

#include <GLFW/glfw3.h>

#include "graphics/camera/CFirstPersonCamera.h"
#include "graphics/camera/CLookAtCamera.h"
#include "graphics/renderer/CRenderer.h"
#include "graphics/resource/CResourceManager.h"
#include "graphics/scene/CScene.h"
#include "graphics/window/CGlfwWindow.h"

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo() {}

int RTRDemo::init()
{
    if (!glfwInit())
    {
        LOG_ERROR("glfwInit() failed.");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_glfw_window = glfwCreateWindow(1024, 768, "RTR Demo", NULL, NULL);
    if (m_glfw_window == nullptr)
    {
        LOG_ERROR("glfwCreateWindow() failed.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_glfw_window);

	// TODO This is just ugly...
#ifndef __APPLE__
    if (flextInit(m_glfw_window) != GL_TRUE)
    {
        glfwTerminate();
        return 1;
    }
#endif

    m_resourceManager = std::make_shared<CResourceManager>();
    m_window = std::make_shared<CGlfwWindow>(m_glfw_window);
    m_renderer = std::make_shared<CRenderer>(m_resourceManager);
    m_scene = std::make_shared<CScene>();
    m_camera = std::make_shared<CLookAtCamera>();

    glfwSetInputMode(m_glfw_window, GLFW_STICKY_KEYS, GL_TRUE);

    return 0;
}

int RTRDemo::run()
{
	// Load cube
	ResourceId cube = m_resourceManager->loadMesh("data/mesh/cube.obj");
	// Error check
	std::string error;
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}
    ResourceId material = m_resourceManager->loadMaterial("data/material/material_test_0.ini");
	// Error check
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}

    // Create scene object with mesh and material
    SceneObjectId sceneObj =
        m_scene->createObject(cube, material, glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f));

	// Set camera
	m_camera->setProjection(90.f, 4.f / 3.f, 1.f, 1000.f);
	m_camera->setView(glm::vec3(40.f, 30.f, 40.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

    do
	{
        m_renderer->draw(*m_scene.get(), *m_camera.get(), *m_window.get());

        glfwSwapBuffers(m_glfw_window);
        glfwPollEvents();

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(m_glfw_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(m_glfw_window) == 0);

    glfwTerminate();

    return 0;
}
