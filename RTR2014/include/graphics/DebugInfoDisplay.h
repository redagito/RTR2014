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

class DebugInfoDisplay : public IDebugInfoDisplay
{
   public:
    DebugInfoDisplay(std::shared_ptr<IResourceManager> resourceManager);

    bool loadFont(const std::string& path);
    void draw(const DebugInfo& info) override;

   private:
    std::shared_ptr<IResourceManager> m_resourceManager;

    std::unique_ptr<ShaderProgram> m_textShader = nullptr;
    std::unique_ptr<ShaderProgram> m_overlayShader = nullptr;
    std::unique_ptr<Texture> m_texture = nullptr;

    std::unique_ptr<VertexArrayObject> m_VAO = nullptr;
    
    std::unique_ptr<VertexBuffer> m_verticesBuffer = nullptr;
    std::unique_ptr<VertexBuffer> m_uvsBuffer = nullptr;
    std::unique_ptr<VertexBuffer> m_overlaysBuffer = nullptr;

    int m_fontSize = 12;
};
