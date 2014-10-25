#include "CDebugInfoDisplay.h"

#include "debug/CDebugInfo.h"
#include "debug/Log.h"

#include "renderer/core/CVertexArrayObject.h"
#include "renderer/core/CVertexBuffer.h"
#include "renderer/CShaderProgram.h"
#include "renderer/CTexture.h"

#include "IResourceManager.h"

bool loadShader(const std::string &path, std::shared_ptr<IResourceManager> &resourceManager,
                std::unique_ptr<CShaderProgram> &resultShader)
{
    ResourceId shaderId = resourceManager->loadShader(path);
    ResourceId vertexShaderId, tessellationControlShaderId, tessellationEvaluationShaderId,
        geometryShaderId, fragmentShaderId;
    if (!resourceManager->getShader(shaderId, vertexShaderId, tessellationControlShaderId,
                                    tessellationEvaluationShaderId, geometryShaderId,
                                    fragmentShaderId))
    {
        LOG_ERROR("Error on retrieving shader code.");
        return false;
    }

    // Load from resoucre manager
    std::string vertexShaderText, fragmentShaderText;
    resourceManager->getString(vertexShaderId, vertexShaderText);
    resourceManager->getString(fragmentShaderId, fragmentShaderText);
    if (vertexShaderText.empty() || fragmentShaderText.empty())
    {
        LOG_ERROR("Error on retrieving shader code.");
        return false;
    }

    TShaderObject<GL_VERTEX_SHADER> vertexShader(vertexShaderText);
    if (!vertexShader.isValid())
    {
        LOG_ERROR("%s", vertexShader.getErrorString().c_str());
        return false;
    }

    TShaderObject<GL_FRAGMENT_SHADER> fragmentShader(fragmentShaderText);
    if (!fragmentShader.isValid())
    {
        LOG_ERROR("%s", fragmentShader.getErrorString().c_str());
        return false;
    }

    resultShader.reset(
        new CShaderProgram(&vertexShader, nullptr, nullptr, nullptr, &fragmentShader));
    return true;
}

CDebugInfoDisplay::CDebugInfoDisplay(std::shared_ptr<IResourceManager> resourceManager)
    : m_resourceManager(resourceManager)
{
    // TODO move this to an init() method

    // TODO remove hardcoded paths here
    loadShader("data/shader/shader_display_text.ini", m_resourceManager, m_textShader);
    loadShader("data/shader/shader_display_overlay.ini", m_resourceManager, m_overlayShader);

    m_VAO.reset(new CVertexArrayObject());
    m_verticesBuffer.reset(new CVertexBuffer(GL_STATIC_DRAW));
    m_uvsBuffer.reset(new CVertexBuffer(GL_STATIC_DRAW));
    m_overlaysBuffer.reset(new CVertexBuffer(GL_STATIC_DRAW));

    // TODO this should not be called in constructor / move to init() method
    loadFont("data/images/font1.png");
}

bool CDebugInfoDisplay::loadFont(const std::string &path)
{
    ResourceId imageId = m_resourceManager->loadImage(path, EColorFormat::RGBA32);
    std::vector<unsigned char> data;
    unsigned int width, height;
    EColorFormat colorFormat;
    if (!m_resourceManager->getImage(imageId, data, width, height, colorFormat))
    {
        LOG_ERROR("Error on retrieving data from image.");
        return false;
    }

    m_texture.reset(new CTexture(data, width, height, colorFormat));

    return true;
}

void textToBuffers(const std::string &text, unsigned int x, unsigned int y, unsigned int size,
                   std::vector<float> &vertices, std::vector<float> &UVs)
{
    size_t length = text.size();

    unsigned int hSize = size * 0.7;
    unsigned int vSize = size;

    for (size_t i = 0; i < length; i++)
    {
        glm::vec2 vertex_up_left = glm::vec2(x + i * hSize, y + vSize);
        glm::vec2 vertex_up_right = glm::vec2(x + i * hSize + hSize, y + vSize);
        glm::vec2 vertex_down_right = glm::vec2(x + i * hSize + hSize, y);
        glm::vec2 vertex_down_left = glm::vec2(x + i * hSize, y);

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

        char character = text[i];
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

void CDebugInfoDisplay::draw(const CDebugInfo &info)
{
    m_VAO->setActive();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    int hOffsetLog = 0;
    int vOffsetLog = info.getLogBufferSize() * m_fontSize;
    int hOffsetKeys = 500;
    int hOffsetValues = 650;
    int vOffsetKeyValues = 600;

    // ===
    // render overlays
    // ===
    
    std::vector<float> overlayVertices;
    
    overlayVertices.push_back(0);
    overlayVertices.push_back(0);
    overlayVertices.push_back(800);
    overlayVertices.push_back(vOffsetLog);
    overlayVertices.push_back(0);
    overlayVertices.push_back(vOffsetLog);
    
    overlayVertices.push_back(0);
    overlayVertices.push_back(0);
    overlayVertices.push_back(800);
    overlayVertices.push_back(0);
    overlayVertices.push_back(800);
    overlayVertices.push_back(vOffsetLog);
    
    overlayVertices.push_back(hOffsetKeys);
    overlayVertices.push_back(vOffsetKeyValues);
    overlayVertices.push_back(hOffsetKeys);
    overlayVertices.push_back(vOffsetKeyValues - m_fontSize * info.getValues().size());
    overlayVertices.push_back(800);
    overlayVertices.push_back(vOffsetKeyValues);
    
    overlayVertices.push_back(800);
    overlayVertices.push_back(vOffsetKeyValues);
    overlayVertices.push_back(hOffsetKeys);
    overlayVertices.push_back(vOffsetKeyValues - m_fontSize * info.getValues().size());
    overlayVertices.push_back(800);
    overlayVertices.push_back(vOffsetKeyValues - m_fontSize * info.getValues().size());
    
    m_overlaysBuffer->setData(overlayVertices);
    
    m_overlayShader->setActive();
    m_overlayShader->setUniform("color", glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_overlaysBuffer->getId());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    
    glDrawArrays(GL_TRIANGLES, 0, overlayVertices.size());
    
    glDisableVertexAttribArray(0);

    // ===
    // render text
    // ===

    std::vector<float> vertices;
    std::vector<float> UVs;

    int lineCount = 0;
    for (const std::string &line : info.getLog())
    {
        textToBuffers(line, hOffsetLog, vOffsetLog - m_fontSize * ++lineCount, m_fontSize, vertices,
                      UVs);
    }

    int valueCount = 0;
    for (const auto &keyValuePair : info.getValues())
    {
        ++valueCount;
        textToBuffers(keyValuePair.first, hOffsetKeys, vOffsetKeyValues - m_fontSize * valueCount,
                      m_fontSize, vertices, UVs);
        textToBuffers(keyValuePair.second, hOffsetValues,
                      vOffsetKeyValues - m_fontSize * valueCount, m_fontSize, vertices, UVs);
    }

    m_verticesBuffer->setData(vertices);
    m_uvsBuffer->setData(UVs);

    m_textShader->setActive();

    m_texture->setActive(0);

    // vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_verticesBuffer->getId());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvsBuffer->getId());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // ===
    // finish
    // ===

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    m_VAO->setInactive();
}