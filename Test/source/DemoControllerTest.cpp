#include <GLFW/glfw3.h>

#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>

#include "animation/AnimationWorld.h"
#include "animation/IAnimationController.h"
#include "debug/DebugInfo.h"
#include "demo/CameraController.h"
#include "demo/DemoController.h"
#include "graphics/ICamera.h"
#include "graphics/IDebugInfoDisplay.h"
#include "graphics/IGraphicsResourceManager.h"
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
              const IGraphicsResourceManager& manager, float deltaTime) override
    {
        ++drawCount;
        lastDeltaTime = deltaTime;
    }

    int drawCount = 0;
    float lastDeltaTime = 0.0f;
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
    void toggleMouseCapture() override
    {
        ++mouseCaptureToggles;
        mouseCaptured = !mouseCaptured;
    }
    bool isMouseCaptured() const override { return mouseCaptured; }
    void pollEvents() override {}
    double getTime() const override { return 0.0; }
    void addListener(IWindowListener* listener) override {}
    void removeListener(IWindowListener* listener) override {}

    int mouseCaptureToggles = 0;
    bool mouseCaptured = false;
    bool vSync = true;
};

class FakeInputProvider : public IInputProvider
{
   public:
    void addInputListener(IInputListener* listener) override {}
    void removeInputListener(IInputListener* listener) override {}
    bool isKeyPressed(int keyCode) override
    {
        auto entry = pressedKeys.find(keyCode);
        return entry != pressedKeys.end() && entry->second;
    }

    void press(int keyCode) { pressedKeys[keyCode] = true; }
    void release(int keyCode) { pressedKeys[keyCode] = false; }

    std::map<int, bool> pressedKeys;
};

class FakeDebugInfoDisplay : public IDebugInfoDisplay
{
   public:
    void draw(const DebugInfo& info, unsigned int drawWidth, unsigned int drawHeight) override
    {
        ++drawCount;
        width = drawWidth;
        height = drawHeight;
    }

    int drawCount = 0;
    unsigned int width = 0;
    unsigned int height = 0;
};

class FakeGraphicsResourceManager : public IGraphicsResourceManager
{
   public:
    Mesh* getMesh(ResourceId) const override { return nullptr; }
    Material* getMaterial(ResourceId) const override { return nullptr; }
    Texture* getTexture(ResourceId) const override { return nullptr; }
    ShaderProgram* getShaderProgram(ResourceId) const override { return nullptr; }
    Texture* getDefaultDiffuseTexture() const override { return nullptr; }
    Texture* getDefaultNormalTexture() const override { return nullptr; }
    Texture* getDefaultSpecularTexture() const override { return nullptr; }
    Texture* getDefaultGlowTexture() const override { return nullptr; }
    Texture* getDefaultAlphaTexture() const override { return nullptr; }
};

// Records how many frames were drawn when it was updated.
class OrderRecordingAnimation : public IAnimationController
{
   public:
    void update(float) override { drawCountAtUpdate = renderer->drawCount; }

    FakeRenderer* renderer = nullptr;
    int drawCountAtUpdate = -1;
};

std::string debugValue(const DebugInfo& info, const std::string& key)
{
    for (const auto& entry : info.getValues())
    {
        if (entry.key == key)
        {
            return entry.status;
        }
    }
    return {};
}

std::string debugValue(const DebugInfo& info, const std::string& key, const std::string& setting)
{
    for (const auto& entry : info.getValues())
    {
        if (entry.key == key && entry.setting == setting)
        {
            return entry.status;
        }
    }
    return {};
}

struct Fixture
{
    Fixture()
        : deferredRenderer(),
          forwardRenderer(),
          scene(),
          camera(std::make_shared<FirstPersonCamera>(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 1.f,
                                                     1.33f, 0.01f, 1000.f)),
          cameraController(std::make_shared<CameraController>()),
          inputProvider(),
          window(),
          debugInfo(),
          debugInfoDisplay(),
          graphicsResourceManager(),
          animationWorld()
    {
        cameraController->setCamera(camera);
        cameraController->setInputProvider(&inputProvider);
        cameraController->setWindow(&window);

        controller = std::make_unique<DemoController>(
            &deferredRenderer, &deferredRenderer, &forwardRenderer, &scene, camera.get(),
            cameraController.get(), &inputProvider, &window, &graphicsResourceManager, &debugInfo,
            &debugInfoDisplay, &animationWorld);
    }

    ~Fixture() {}

    FakeRenderer deferredRenderer;
    FakeRenderer forwardRenderer;
    Scene scene;
    std::shared_ptr<FirstPersonCamera> camera;
    FakeInputProvider inputProvider;
    FakeWindow window;
    DebugInfo debugInfo;
    FakeDebugInfoDisplay debugInfoDisplay;
    FakeGraphicsResourceManager graphicsResourceManager;
    AnimationWorld animationWorld;
    // Declared last so they are destroyed first: the camera controller
    // deregisters from the window and the input provider on destruction.
    std::shared_ptr<CameraController> cameraController;
    std::unique_ptr<DemoController> controller;
};
}  // namespace

TEST_CASE("Demo controller draws the initial renderer", "[demo]")
{
    Fixture f;

    f.controller->update(0.016f);

    REQUIRE(f.deferredRenderer.drawCount == 1);
    REQUIRE(f.forwardRenderer.drawCount == 0);
}

TEST_CASE("Demo controller passes the frame time to the renderer", "[demo]")
{
    Fixture f;

    f.controller->update(0.025f);

    REQUIRE(f.deferredRenderer.lastDeltaTime == 0.025f);
}

TEST_CASE("Demo controller respects a forward renderer passed as initial", "[demo]")
{
    Fixture f;

    DemoController controller(&f.forwardRenderer, &f.deferredRenderer, &f.forwardRenderer, &f.scene,
                              f.camera.get(), f.cameraController.get(), &f.inputProvider, &f.window,
                              &f.graphicsResourceManager, &f.debugInfo, &f.debugInfoDisplay,
                              &f.animationWorld);

    controller.update(0.016f);

    REQUIRE(f.forwardRenderer.drawCount == 1);
    REQUIRE(f.deferredRenderer.drawCount == 0);
}

TEST_CASE("Demo controller toggles the renderer on F3", "[demo]")
{
    Fixture f;

    f.inputProvider.press(GLFW_KEY_F3);
    f.controller->update(0.016f);
    REQUIRE(f.forwardRenderer.drawCount == 1);
    REQUIRE(f.deferredRenderer.drawCount == 0);

    f.inputProvider.release(GLFW_KEY_F3);
    f.controller->update(0.4f);
    const int forwardDrawCount = f.forwardRenderer.drawCount;

    f.inputProvider.press(GLFW_KEY_F3);
    f.controller->update(0.016f);
    REQUIRE(f.deferredRenderer.drawCount == 1);
    REQUIRE(f.forwardRenderer.drawCount == forwardDrawCount);
}

TEST_CASE("Demo controller toggles the debug info display on F2", "[demo]")
{
    Fixture f;

    f.controller->update(0.016f);
    REQUIRE(f.debugInfoDisplay.drawCount == 0);

    f.inputProvider.press(GLFW_KEY_F2);
    f.controller->update(0.016f);
    REQUIRE(f.debugInfoDisplay.drawCount == 1);
    REQUIRE(f.debugInfoDisplay.width == 800);
    REQUIRE(f.debugInfoDisplay.height == 600);
    REQUIRE(f.deferredRenderer.drawCount == 2);
    REQUIRE(f.forwardRenderer.drawCount == 0);
}

TEST_CASE("Debug overlay shows key bindings and active settings", "[demo]")
{
    Fixture f;
    f.inputProvider.press(GLFW_KEY_F2);
    f.controller->update(0.016f);

    REQUIRE(debugValue(f.debugInfo, "F1") == "Off");
    REQUIRE(debugValue(f.debugInfo, "F2") == "On");
    REQUIRE(debugValue(f.debugInfo, "F3") == "Deferred");
    REQUIRE(debugValue(f.debugInfo, "F4") == "On");
    REQUIRE(debugValue(f.debugInfo, "F5") == "Manual");
    REQUIRE(debugValue(f.debugInfo, "R") == "Final");
    REQUIRE(debugValue(f.debugInfo, "T") == "None");
    REQUIRE(debugValue(f.debugInfo, "Z") == "On");
    REQUIRE(debugValue(f.debugInfo, "U") == "On");
    REQUIRE(debugValue(f.debugInfo, "I") == "On");
    REQUIRE(debugValue(f.debugInfo, "O") == "On");

    f.inputProvider.release(GLFW_KEY_F2);
    f.inputProvider.press(GLFW_KEY_F1);
    f.controller->update(0.016f);
    REQUIRE(debugValue(f.debugInfo, "F1") == "On");

    f.inputProvider.release(GLFW_KEY_F1);
    f.inputProvider.press(GLFW_KEY_F3);
    f.controller->update(0.016f);
    REQUIRE(debugValue(f.debugInfo, "F3") == "Forward");

    FeatureInfo& features = f.camera->getFeatureInfoForWrite();
    features.renderMode = RenderMode::Normals;
    features.fogType = FogType::Exp;
    features.shadowsActive = false;
    features.fxaaActive = false;
    features.dofActive = false;
    features.normalMappingActive = false;
    f.controller->update(0.016f);

    REQUIRE(debugValue(f.debugInfo, "R") == "Normals");
    REQUIRE(debugValue(f.debugInfo, "T") == "Exp");
    REQUIRE(debugValue(f.debugInfo, "Z") == "Off");
    REQUIRE(debugValue(f.debugInfo, "U") == "Off");
    REQUIRE(debugValue(f.debugInfo, "I") == "Off");
    REQUIRE(debugValue(f.debugInfo, "O") == "Off");
}

TEST_CASE("Debug overlay shows camera and mouse values", "[demo]")
{
    Fixture f;
    f.cameraController->handleMouseMovementEvent(120, 80);
    f.camera->lookAt(glm::vec3(0.f), glm::vec3(1.f, 0.5f, 0.f));
    f.inputProvider.press(GLFW_KEY_F2);

    f.controller->update(0.016f);

    REQUIRE(debugValue(f.debugInfo, "Camera", "Position") == "(0.00, 0.00, 0.00)");
    REQUIRE(debugValue(f.debugInfo, "Mouse", "X") == "120");
    REQUIRE(debugValue(f.debugInfo, "Mouse", "Y") == "80");
    REQUIRE(debugValue(f.debugInfo, "Yaw") == "90.00 deg");
    REQUIRE(debugValue(f.debugInfo, "Pitch") == "26.57 deg");
    REQUIRE(debugValue(f.debugInfo, "Frame") == "1");
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

TEST_CASE("Demo controller requests a screenshot on F12", "[demo]")
{
    Fixture f;

    REQUIRE_FALSE(f.controller->consumeScreenshotRequest());

    f.inputProvider.press(GLFW_KEY_F12);
    f.controller->update(0.016f);
    REQUIRE(f.controller->consumeScreenshotRequest());

    f.controller->update(0.016f);
    REQUIRE_FALSE(f.controller->consumeScreenshotRequest());
}

TEST_CASE("Demo controller survives a missing renderer", "[demo]")
{
    Fixture f;

    DemoController controller(nullptr, nullptr, nullptr, &f.scene, f.camera.get(),
                              f.cameraController.get(), &f.inputProvider, &f.window,
                              &f.graphicsResourceManager, &f.debugInfo, &f.debugInfoDisplay,
                              &f.animationWorld);

    // Must not crash
    controller.update(0.016f);

    REQUIRE(f.deferredRenderer.drawCount == 0);
    REQUIRE(f.forwardRenderer.drawCount == 0);
}

TEST_CASE("Demo controller keeps the renderer when the deferred one is missing", "[demo]")
{
    Fixture f;

    DemoController controller(&f.forwardRenderer, nullptr, &f.forwardRenderer, &f.scene,
                              f.camera.get(), f.cameraController.get(), &f.inputProvider, &f.window,
                              &f.graphicsResourceManager, &f.debugInfo, &f.debugInfoDisplay,
                              &f.animationWorld);

    f.inputProvider.press(GLFW_KEY_F3);
    controller.update(0.016f);

    REQUIRE(f.forwardRenderer.drawCount == 1);
}

TEST_CASE("Demo controller keeps the frame rate after a long frame", "[demo]")
{
    Fixture f;
    f.inputProvider.press(GLFW_KEY_F2);
    f.controller->update(0.016f);
    // The key is only pressed once, the toggle would fire again otherwise.
    f.inputProvider.release(GLFW_KEY_F2);

    // A frame that takes five seconds
    f.controller->update(5.0f);
    // The frames after it are counted in the new second
    f.controller->update(0.5f);
    f.controller->update(0.5f);

    const int fps = std::stoi(debugValue(f.debugInfo, "FPS"));
    REQUIRE(fps == 2);
}

TEST_CASE("Demo controller animates the scene before it draws it", "[demo]")
{
    Fixture f;

    auto animation = std::make_shared<OrderRecordingAnimation>();
    animation->renderer = &f.deferredRenderer;
    f.animationWorld.addAnimationController(animation);

    f.controller->update(0.016f);

    // The animation must not see the frame that is drawn after it.
    REQUIRE(animation->drawCountAtUpdate == 0);
}