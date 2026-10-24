#pragma once

#include <memory>
#include <string>
#include <vector>

#include "graphics/IDebugInfoDisplay.h"

class DebugInfo;
class ShaderProgram;
class Texture;
class VertexArrayObject;
class VertexBuffer;
class IResourceManager;

/**
 * \brief Pixel geometry of the debug panels.
 *
 * The rows are drawn from the top of the window down to the log panel, which
 * keeps the bottom of the screen for the log.
 */
struct DebugPanelLayout
{
    int keyX;        /**< Left edge of the shortcut column. */
    int settingX;    /**< Left edge of the setting column. */
    int statusX;     /**< Left edge of the status column. */
    int logBottom;   /**< Height of the log panel. */
    int rowsTop;     /**< Y coordinate the first row is drawn below. */
    int panelWidth;  /**< Width of the panels. */
    int visibleRows; /**< Number of rows that fit above the log panel. */
};

/**
 * \brief Computes the debug panel layout for a window size.
 *
 * Rows that would overlap the log panel are clipped.
 */
DebugPanelLayout computeDebugPanelLayout(int fontSize, int logLineCount, int valueCount,
                                         unsigned int width, unsigned int height);

class DebugInfoDisplay : public IDebugInfoDisplay
{
   public:
    DebugInfoDisplay(std::shared_ptr<IResourceManager> resourceManager);

    bool loadFont(const std::string& path);
    void draw(const DebugInfo& info, unsigned int width, unsigned int height) override;

   private:
    std::shared_ptr<IResourceManager> m_resourceManager;

    std::unique_ptr<ShaderProgram> m_textShader = nullptr;
    std::unique_ptr<ShaderProgram> m_overlayShader = nullptr;
    std::unique_ptr<Texture> m_texture = nullptr;

    std::unique_ptr<VertexArrayObject> m_VAO = nullptr;
    std::unique_ptr<VertexArrayObject> m_overlayVAO = nullptr;

    std::unique_ptr<VertexBuffer> m_verticesBuffer = nullptr;
    std::unique_ptr<VertexBuffer> m_uvsBuffer = nullptr;
    std::unique_ptr<VertexBuffer> m_overlaysBuffer = nullptr;

    int m_fontSize = 12;
};
