#include "graphics/DebugInfoDisplay.h"

#include <spdlog/spdlog.h>

#include <algorithm>

#include "debug/DebugInfo.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/core/VertexArrayObject.h"
#include "graphics/renderer/core/VertexBuffer.h"
#include "resource/IResourceManager.h"

bool loadShader(const std::string& path, std::shared_ptr<IResourceManager>& resourceManager,
                std::unique_ptr<ShaderProgram>& resultShader)
{
    ResourceId shaderId = resourceManager->loadShader(path);
    ResourceId vertexShaderId, tessellationControlShaderId, tessellationEvaluationShaderId,
        geometryShaderId, fragmentShaderId;
    if (!resourceManager->getShader(shaderId, vertexShaderId, tessellationControlShaderId,
                                    tessellationEvaluationShaderId, geometryShaderId,
                                    fragmentShaderId))
    {
        spdlog::error("Error on retrieving shader code.");
        return false;
    }

    // Load from resoucre manager
    std::string vertexShaderText, fragmentShaderText;
    if (!resourceManager->getString(vertexShaderId, vertexShaderText) ||
        !resourceManager->getString(fragmentShaderId, fragmentShaderText) ||
        vertexShaderText.empty() || fragmentShaderText.empty())
    {
        spdlog::error("Error on retrieving shader code.");
        return false;
    }

    ShaderObject<GL_VERTEX_SHADER> vertexShader(vertexShaderText);
    if (!vertexShader.isValid())
    {
        spdlog::error("{}", vertexShader.getErrorString().c_str());
        return false;
    }

    ShaderObject<GL_FRAGMENT_SHADER> fragmentShader(fragmentShaderText);
    if (!fragmentShader.isValid())
    {
        spdlog::error("{}", fragmentShader.getErrorString().c_str());
        return false;
    }

    resultShader.reset(
        new ShaderProgram(&vertexShader, nullptr, nullptr, nullptr, &fragmentShader));
    return true;
}

DebugInfoDisplay::DebugInfoDisplay(std::shared_ptr<IResourceManager> resourceManager)
    : m_resourceManager(resourceManager)
{
    // TODO move this to an init() method

    // TODO remove hardcoded paths here
    if (!loadShader("data/shader/shader_display_text.ini", m_resourceManager, m_textShader))
    {
        spdlog::error("The debug info display is not available, its shader could not be loaded.");
    }
    if (!loadShader("data/shader/shader_display_overlay.ini", m_resourceManager, m_overlayShader))
    {
        spdlog::error(
            "The debug info display is not available, its overlay shader could not be "
            "loaded.");
    }

    m_VAO.reset(new VertexArrayObject());
    m_overlayVAO.reset(new VertexArrayObject());
    m_verticesBuffer.reset(new VertexBuffer(GL_STATIC_DRAW));
    m_uvsBuffer.reset(new VertexBuffer(GL_STATIC_DRAW));
    m_overlaysBuffer.reset(new VertexBuffer(GL_STATIC_DRAW));

    // The vertex attributes are set up once, the buffers are refilled per frame
    glVertexArrayVertexBuffer(m_VAO->getId(), 0, m_verticesBuffer->getId(), 0, 2 * sizeof(float));
    glVertexArrayAttribFormat(m_VAO->getId(), 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VAO->getId(), 0, 0);
    glEnableVertexArrayAttrib(m_VAO->getId(), 0);

    glVertexArrayVertexBuffer(m_VAO->getId(), 1, m_uvsBuffer->getId(), 0, 2 * sizeof(float));
    glVertexArrayAttribFormat(m_VAO->getId(), 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VAO->getId(), 1, 1);
    glEnableVertexArrayAttrib(m_VAO->getId(), 1);

    glVertexArrayVertexBuffer(m_overlayVAO->getId(), 0, m_overlaysBuffer->getId(), 0,
                              2 * sizeof(float));
    glVertexArrayAttribFormat(m_overlayVAO->getId(), 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_overlayVAO->getId(), 0, 0);
    glEnableVertexArrayAttrib(m_overlayVAO->getId(), 0);

    // TODO this should not be called in constructor / move to init() method
    if (!loadFont("data/images/font1.png"))
    {
        spdlog::error("The debug info display is not available, its font could not be loaded.");
    }
}

bool DebugInfoDisplay::loadFont(const std::string& path)
{
    // The font is a mask for the text, its values are not a color.
    ResourceId imageId =
        m_resourceManager->loadImage(path, ColorFormat::RGBA32, ColorSpace::Linear);
    std::vector<unsigned char> data;
    unsigned int width, height;
    ColorFormat colorFormat;
    ColorSpace colorSpace;
    if (!m_resourceManager->getImage(imageId, data, width, height, colorFormat, colorSpace))
    {
        spdlog::error("Error on retrieving data from image.");
        return false;
    }

    m_texture.reset(new Texture(data, width, height, colorFormat, colorSpace, false));

    return true;
}

void textToBuffers(const std::string& text, int x, int y, int size, std::vector<float>& vertices,
                   std::vector<float>& UVs)
{
    size_t length = text.size();

    const int hSize = (int)(size * 0.6f);
    const int vSize = size;

    for (size_t i = 0; i < length; i++)
    {
        const int glyphX = x + (int)i * hSize;
        glm::vec2 vertex_up_left = glm::vec2(glyphX, y + vSize);
        glm::vec2 vertex_up_right = glm::vec2(glyphX + hSize, y + vSize);
        glm::vec2 vertex_down_right = glm::vec2(glyphX + hSize, y);
        glm::vec2 vertex_down_left = glm::vec2(glyphX, y);

        vertices.push_back(vertex_up_left.x);
        vertices.push_back(vertex_up_left.y);
        vertices.push_back(vertex_down_left.x);
        vertices.push_back(vertex_down_left.y);
        vertices.push_back(vertex_up_right.x);
        vertices.push_back(vertex_up_right.y);

        vertices.push_back(vertex_down_right.x);
        vertices.push_back(vertex_down_right.y);
        vertices.push_back(vertex_up_right.x);
        vertices.push_back(vertex_up_right.y);
        vertices.push_back(vertex_down_left.x);
        vertices.push_back(vertex_down_left.y);

        // The font atlas is indexed by the unsigned byte value.
        const unsigned char character = static_cast<unsigned char>(text[i]);
        float uv_x = (character % 16) / 16.0f;
        float uv_y = (character / 16) / 16.0f;

        glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
        glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
        glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
        glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));

        UVs.push_back(uv_up_left.x);
        UVs.push_back(uv_up_left.y);
        UVs.push_back(uv_down_left.x);
        UVs.push_back(uv_down_left.y);
        UVs.push_back(uv_up_right.x);
        UVs.push_back(uv_up_right.y);

        UVs.push_back(uv_down_right.x);
        UVs.push_back(uv_down_right.y);
        UVs.push_back(uv_up_right.x);
        UVs.push_back(uv_up_right.y);
        UVs.push_back(uv_down_left.x);
        UVs.push_back(uv_down_left.y);
    }
}

DebugPanelLayout computeDebugPanelLayout(int fontSize, int logLineCount, int valueCount,
                                         unsigned int width, unsigned int height)
{
    DebugPanelLayout layout;
    layout.keyX = 360;
    layout.settingX = 430;
    layout.statusX = 600;
    layout.logBottom = fontSize * logLineCount;
    layout.rowsTop = static_cast<int>(height);
    layout.panelWidth = static_cast<int>(width);

    const int available = layout.rowsTop - layout.logBottom;
    const bool fits = fontSize > 0 && available > 0 && valueCount > 0;
    layout.visibleRows = fits ? std::min(valueCount, available / fontSize) : 0;
    return layout;
}

void DebugInfoDisplay::draw(const DebugInfo& info, unsigned int width, unsigned int height)
{
    if (m_overlayShader == nullptr || m_textShader == nullptr || m_texture == nullptr)
    {
        return;
    }

    // The state of the renderer is restored when the overlay is done.
    const bool depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    const bool blendEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // The values are read once, the getters copy them.
    const std::list<std::string> log = info.getLog();
    const auto values = info.getValues();

    const DebugPanelLayout layout =
        computeDebugPanelLayout(m_fontSize, static_cast<int>(info.getLogBufferSize()),
                                static_cast<int>(values.size()), width, height);
    const int hOffsetLog = 0;

    // ===
    // render overlays
    // ===

    std::vector<float> overlayVertices;

    overlayVertices.push_back(0.f);
    overlayVertices.push_back(0.f);
    overlayVertices.push_back((float)layout.panelWidth);
    overlayVertices.push_back((float)layout.logBottom);
    overlayVertices.push_back(0.f);
    overlayVertices.push_back((float)layout.logBottom);

    overlayVertices.push_back(0.f);
    overlayVertices.push_back(0.f);
    overlayVertices.push_back((float)layout.panelWidth);
    overlayVertices.push_back(0.f);
    overlayVertices.push_back((float)layout.panelWidth);
    overlayVertices.push_back((float)layout.logBottom);

    if (layout.panelWidth > layout.keyX && layout.rowsTop > layout.logBottom)
    {
        overlayVertices.push_back((float)layout.keyX);
        overlayVertices.push_back((float)layout.logBottom);
        overlayVertices.push_back((float)layout.panelWidth);
        overlayVertices.push_back((float)layout.rowsTop);
        overlayVertices.push_back((float)layout.keyX);
        overlayVertices.push_back((float)layout.rowsTop);

        overlayVertices.push_back((float)layout.keyX);
        overlayVertices.push_back((float)layout.logBottom);
        overlayVertices.push_back((float)layout.panelWidth);
        overlayVertices.push_back((float)layout.logBottom);
        overlayVertices.push_back((float)layout.panelWidth);
        overlayVertices.push_back((float)layout.rowsTop);
    }

    m_overlaysBuffer->setData(overlayVertices);

    m_overlayShader->setActive();
    m_overlayShader->setUniform("color", glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));

    m_overlayVAO->setActive();
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(overlayVertices.size() / 2));
    m_overlayVAO->setInactive();

    // ===
    // render text
    // ===

    std::vector<float> vertices;
    std::vector<float> UVs;

    int lineCount = 0;
    for (const std::string& line : log)
    {
        textToBuffers(line, hOffsetLog, layout.logBottom - m_fontSize * ++lineCount, m_fontSize,
                      vertices, UVs);
    }

    int valueCount = 0;
    for (const DebugValue& value : values)
    {
        if (valueCount >= layout.visibleRows)
        {
            break;
        }

        ++valueCount;
        const int y = layout.rowsTop - m_fontSize * valueCount;
        textToBuffers(value.key, layout.keyX, y, m_fontSize, vertices, UVs);
        textToBuffers(value.setting, layout.settingX, y, m_fontSize, vertices, UVs);
        textToBuffers(value.status, layout.statusX, y, m_fontSize, vertices, UVs);
    }

    m_verticesBuffer->setData(vertices);
    m_uvsBuffer->setData(UVs);

    m_textShader->setActive();

    m_texture->setActive(0);

    m_VAO->setActive();
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 2));
    m_VAO->setInactive();

    // ===
    // finish
    // ===

    glDisable(GL_BLEND);
    if (depthTestEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    if (blendEnabled)
    {
        glEnable(GL_BLEND);
    }
}
