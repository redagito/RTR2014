#pragma once

#include <string>

class CameraController;
class DebugInfo;
class IControllableCamera;
class IRenderer;
class IWindow;

/**
 * \brief Builds the JSON metadata that is written beside a screenshot.
 *
 * The metadata describes the state the screenshot was taken in: the image file,
 * the frame counter, the window and renderer, the camera, the last mouse
 * position, the camera sequence and the debug rows.
 *
 * \param imageFile Name of the screenshot image file.
 * \param frame Number of the frame the screenshot was taken from.
 * \param renderer Renderer that drew the frame.
 * \param window Window the frame was rendered into.
 * \param camera Camera the frame was rendered from.
 * \param cameraController Controller that drives the camera.
 * \param debugInfo Debug rows to include.
 * \return The JSON document as a string with two-space indentation and no
 * trailing newline.
 */
std::string buildScreenshotMetadata(const std::string& imageFile, unsigned long long frame,
                                    const IRenderer& renderer, const IWindow& window,
                                    const IControllableCamera& camera,
                                    const CameraController& cameraController,
                                    const DebugInfo& debugInfo);
