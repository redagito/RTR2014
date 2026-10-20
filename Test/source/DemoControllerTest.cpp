#include <catch2/catch_test_macros.hpp>

#include <map>
#include <memory>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include "animation/AnimationWorld.h"
#include "debug/DebugInfo.h"
#include "demo/CameraController.h"
#include "demo/DemoController.h"
#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IDebugInfoDisplay.h"
#include "graphics/IRenderer.h"
#include "graphics/IScene.h"
#include "graphics/IWindow.h"
#include "graphics/camera/FirstPersonCamera.h"
#include "graphics/scene/Scene.h"
#include "input/IInputProvider.h"

namespace
{
class FakeRenderer : public IRenderer
{
   public:
    void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
              const IGraphicsResourceManager& manager) override
    {
        ++drawCount;
    }

    int drawCount = 0;
};

class FakeWindow : public IWindow
{
   public:
    unsigned int getWidth() const override { return 800; }
    unsigned int getHeight() const override { return 600; }
    void setActive() const override {}
    bool isOpen() const override { return true; }
    void swapBuffer() override {}
    void setVSync(bool enabled) override { vSync = enabled; }
    void toggleMouseCapture() override { ++mouseCaptureToggles; }
    void pollEvents() override {}
    double getTime() const override { return 0.0; }
    void addListener(IWindowListener* listener) override {}
    void removeListener(IWindowListener* listener) override {}

    int mouseCaptureToggles = 0;
    bool vSync = true;
};

class FakeInputProvider : public IInputProvider
{
   public:
    void addInputListener(IInputListener* listener) override {}
    void removeInputListener(IInputListener* listener) override {}
    bool isKeyPressed(int keyCode) override { return pressedKeys.count(keyCode) > 0; }

    void press(int keyCode) { pressedKeys[keyCode] = true; }

    std::map<int, bool> pressedKeys;
};

class FakeDebugInfoDisplay : public IDebugInfoDisplay
{
   public:
    void draw(const DebugInfo& info) override { ++drawCount; }

    int drawCount = 0;
};

struct Fixture
{
    Fixture()
        : deferredRenderer(),
          forwardRenderer(),
          scene(),
          camera(std::make_shared<FirstPersonCamera>(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f),
                                                   glm::vec3(0.f, 1.f, 0.f), 1.f, 1.33f, 0.01f,
                                                   1000.f)),
          cameraController(std::make_shared<CameraController>()),
          inputProvider(),
          window(),
          debugInfo(),
          debugInfoDisplay(),
          animationWorld()
    {
        cameraController->setCamera(camera);
        cameraController->setInputProvider(&inputProvider);
        cameraController->setWindow(&window);

        controller = std::make_unique<DemoController>(&deferredRenderer, &deferredRenderer,
                                                      &forwardRenderer, &scene, camera.get(),
                                                      cameraController.get(), &inputProvider,
                                                      &window, nullptr, &debugInfo,
                                                      &debugInfoDisplay, &animationWorld);
    }

    ~Fixture()
    {
    }

    FakeRenderer deferredRenderer;
    FakeRenderer forwardRenderer;
    Scene scene;
    std::shared_ptr<FirstPersonCamera> camera;
    FakeInputProvider inputProvider;
    FakeWindow window;
    DebugInfo debugInfo;
    FakeDebugInfoDisplay debugInfoDisplay;
    AnimationWorld animationWorld;
    // Declared last so they are destroyed first: the camera controller
    // deregisters from the window and the input provider on destruction.
    std::shared_ptr<CameraController> cameraController;
    std::unique_ptr<DemoController> controller;
};
} // namespace

TEST_CASE("Demo controller draws the initial renderer", "[demo]")
{
    Fixture f;

    f.controller->update(0.016f);

    REQUIRE(f.deferredRenderer.drawCount == 1);
    REQUIRE(f.forwardRenderer.drawCount == 0);
}

TEST_CASE("Demo controller respects a forward renderer passed as initial", "[demo]")
{
    Fixture f;

    DemoController controller(&f.forwardRenderer, &f.deferredRenderer, &f.forwardRenderer,
                              &f.scene, f.camera.get(), f.cameraController.get(), &f.inputProvider,
                              &f.window, nullptr, &f.debugInfo, &f.debugInfoDisplay,
                              &f.animationWorld);

    controller.update(0.016f);

    REQUIRE(f.forwardRenderer.drawCount == 1);
    REQUIRE(f.deferredRenderer.drawCount == 0);
}

TEST_CASE("Demo controller switches renderer on F2 and F3", "[demo]")
{
    Fixture f;

    f.inputProvider.press(GLFW_KEY_F3);
    f.controller->update(0.016f);
    REQUIRE(f.forwardRenderer.drawCount == 1);
    REQUIRE(f.deferredRenderer.drawCount == 0);

    f.inputProvider.press(GLFW_KEY_F2);
    f.controller->update(0.016f);
    REQUIRE(f.deferredRenderer.drawCount == 1);
    REQUIRE(f.forwardRenderer.drawCount == 1);
}

TEST_CASE("Demo controller toggles the debug info display on key 1", "[demo]")
{
    Fixture f;

    f.controller->update(0.016f);
    REQUIRE(f.debugInfoDisplay.drawCount == 0);

    f.inputProvider.press(GLFW_KEY_1);
    f.controller->update(0.016f);
    REQUIRE(f.debugInfoDisplay.drawCount == 1);
}

TEST_CASE("Demo controller toggles mouse capture on F1", "[demo]")
{
    Fixture f;

    f.controller->update(0.016f);
    REQUIRE(f.window.mouseCaptureToggles == 0);

    f.inputProvider.press(GLFW_KEY_F1);
    f.controller->update(0.016f);
    REQUIRE(f.window.mouseCaptureToggles == 1);
}

TEST_CASE("Demo controller toggles vsync on F4", "[demo]")
{
    Fixture f;

    f.controller->update(0.016f);
    REQUIRE(f.window.vSync == true);

    f.inputProvider.press(GLFW_KEY_F4);
    f.controller->update(0.016f);
    REQUIRE(f.window.vSync == false);
}