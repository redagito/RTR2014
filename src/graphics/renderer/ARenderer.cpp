#include "ARenderer.h"

#include <cassert>
#include <string>

#include "graphics/IResourceManager.h"
#include "debug/Log.h"

ARenderer::ARenderer(const std::shared_ptr<IResourceManager>& resourceManager)
    : m_resourceManager(resourceManager),
      m_defaultNormalTexture(nullptr),
      m_defaultSpecularTexture(nullptr),
      m_defaultGlowTexture(nullptr),
      m_defaultAlphaTexture(nullptr)
{
    // Register self as resource listener
    m_resourceManager->addResourceListener(this);
    // Create default textures
    initDefaultTextures();
    return;
}

ARenderer::~ARenderer()
{
    // Remove resource listener
    m_resourceManager->removeResourceListener(this);
}

void ARenderer::onAttach(IResourceManager* resourceManager)
{
    // TODO Implement
    assert(resourceManager == m_resourceManager.get());
    return;
}

void ARenderer::onDetach(IResourceManager* resourceManager)
{
    // TODO Implement
    assert(resourceManager == m_resourceManager.get());
    return;
}

void ARenderer::notify(EResourceType type, ResourceId id, EListenerEvent event,
                       IResourceManager* resourceManager)
{
    // Resolve resource type
    switch (type)
    {
    case EResourceType::Image:
        handleImageEvent(id, event, resourceManager);
        break;
    case EResourceType::Material:
        handleMaterialEvent(id, event, resourceManager);
        break;
    case EResourceType::Mesh:
        handleMeshEvent(id, event, resourceManager);
        break;
    case EResourceType::Shader:
        handleShaderEvent(id, event, resourceManager);
        break;
    case EResourceType::String:
        handleStringEvent(id, event, resourceManager);
        break;
    default:
        assert(false && "Unknown resource type.");
        break;
    }
}

void ARenderer::draw(CMesh* mesh)
{
    mesh->getVertexArray()->setActive();

    // Set primitive draw mode
    GLenum mode = CMesh::toGLPrimitive(mesh->getPrimitiveType());
    unsigned int primitiveSize = CMesh::getPrimitiveSize(mesh->getPrimitiveType());

    // Decide on draw method based on the stored data
    if (mesh->hasIndexBuffer())
    {
        // Indexed draw, faster
        mesh->getIndexBuffer()->setActive();
        glDrawElements(mode, mesh->getIndexBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
        mesh->getIndexBuffer()->setInactive();
    }
    else
    {
        // Slowest draw method
        glDrawArrays(mode, 0, mesh->getVertexBuffer()->getSize() / primitiveSize);
    }
    mesh->getVertexArray()->setInactive();
}

IResourceManager* ARenderer::getResourceManager() const { return m_resourceManager.get(); }

void ARenderer::initDefaultTextures()
{
    // Default diffuse texture is deep pink to signal errors/missing textures
    m_defaultDiffuseTexture.reset(new CTexture({238, 18, 137}, 1, 1, EColorFormat::RGB24, 0));

    // Default normal texture with straight/non-perturbed normals
    // Discussion here:
    // http://www.gameartisans.org/forums/threads/1985-Normal-Map-RGB-127-127-255-or-128-128-255
    m_defaultNormalTexture.reset(new CTexture({128, 128, 255}, 1, 1, EColorFormat::RGB24, 0));

    // Default specular texture is black (no specular highlights)
    m_defaultSpecularTexture.reset(new CTexture({0}, 1, 1, EColorFormat::GreyScale8));

    // Default glow texture is black (no glow)
    m_defaultGlowTexture.reset(new CTexture({0}, 1, 1, EColorFormat::GreyScale8));

    // Default alpha texture is white (completely opaque)
    m_defaultAlphaTexture.reset(new CTexture({255}, 1, 1, EColorFormat::GreyScale8));
}

CMesh* ARenderer::getMesh(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_meshes.find(id);

    // Id must exist
    // TODO Allow mesh loading if not found?
    assert(iter != m_meshes.end());
    return iter->second.get();
}

CMaterial* ARenderer::getMaterial(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_materials.find(id);

    // Id must exist
    // TODO Allow material loading if not found?
    assert(iter != m_materials.end());
    return iter->second.get();
}

CTexture* ARenderer::getTexture(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_textures.find(id);

    // Id must exist
    // TODO Allow texture loading if not found?
    assert(iter != m_textures.end());
    return iter->second.get();
}

CShaderProgram* ARenderer::getShaderProgram(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_shaderPrograms.find(id);

    // Id must exist
    // TODO Allow shader loading if not found?
    assert(iter != m_shaderPrograms.end());
    return iter->second.get();
}

TShaderObject<GL_VERTEX_SHADER>* ARenderer::getVertexShaderObject(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_vertexShader.find(id);

    // Id must exist
    // TODO Allow shader loading if not found?
    assert(iter != m_vertexShader.end());
    return iter->second.get();
}

TShaderObject<GL_TESS_CONTROL_SHADER>* ARenderer::getTessControlShaderObject(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_tessConstrolShader.find(id);

    // Id must exist
    // TODO Allow shader loading if not found?
    assert(iter != m_tessConstrolShader.end());
    return iter->second.get();
}

TShaderObject<GL_TESS_EVALUATION_SHADER>* ARenderer::getTessEvalShaderObject(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_tessEvalShader.find(id);

    // Id must exist
    // TODO Allow shader loading if not found?
    assert(iter != m_tessEvalShader.end());
    return iter->second.get();
}

TShaderObject<GL_GEOMETRY_SHADER>* ARenderer::getGeometryShaderObject(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_geometryShader.find(id);

    // Id must exist
    // TODO Allow shader loading if not found?
    assert(iter != m_geometryShader.end());
    return iter->second.get();
}

TShaderObject<GL_FRAGMENT_SHADER>* ARenderer::getFragmentShaderObject(ResourceId id) const
{
    // Invalid id
    if (id == -1)
    {
        return nullptr;
    }
    // Search for id
    auto iter = m_fragmentShader.find(id);

    // Id must exist
    // TODO Allow shader loading if not found?
    assert(iter != m_fragmentShader.end());
    return iter->second.get();
}

bool ARenderer::loadVertexShader(ResourceId id, IResourceManager* resourceManager)
{
    // Unused id
    if (id == -1)
    {
        return true;
    }
    // Already loaded
    if (m_vertexShader.count(id) != 0)
    {
        return true;
    }
    // Load from resoucre manager
    std::string text;
    resourceManager->getString(id, text);
    if (text.empty())
    {
        return false;
    }
    std::unique_ptr<TShaderObject<GL_VERTEX_SHADER>> shader(
        new TShaderObject<GL_VERTEX_SHADER>(text));
    // Check compile error
    if (!shader->isValid())
    {
        LOG_ERROR("%s", shader->getErrorString().c_str());
        return false;
    }
    // Move to map
    m_vertexShader[id] = std::move(shader);
    return true;
}

bool ARenderer::loadTessControlShader(ResourceId id, IResourceManager* resourceManager)
{
    // Unused id
    if (id == -1)
    {
        return true;
    }
    // Already loaded
    if (m_tessConstrolShader.count(id) != 0)
    {
        return true;
    }
    // Load from resoucre manager
    std::string text;
    resourceManager->getString(id, text);
    if (text.empty())
    {
        return false;
    }
    std::unique_ptr<TShaderObject<GL_TESS_CONTROL_SHADER>> shader(
        new TShaderObject<GL_TESS_CONTROL_SHADER>(text));
    // Check compile error
    if (!shader->isValid())
    {
        LOG_ERROR("%s", shader->getErrorString().c_str());
        return false;
    }
    // Move to map
    m_tessConstrolShader[id] = std::move(shader);
    return true;
}

bool ARenderer::loadTessEvalShader(ResourceId id, IResourceManager* resourceManager)
{
    // Unused id
    if (id == -1)
    {
        return true;
    }
    // Already loaded
    if (m_tessEvalShader.count(id) != 0)
    {
        return true;
    }
    // Load from resoucre manager
    std::string text;
    resourceManager->getString(id, text);
    if (text.empty())
    {
        return false;
    }
    std::unique_ptr<TShaderObject<GL_TESS_EVALUATION_SHADER>> shader(
        new TShaderObject<GL_TESS_EVALUATION_SHADER>(text));
    // Check compile error
    if (!shader->isValid())
    {
        LOG_ERROR("%s", shader->getErrorString().c_str());
        return false;
    }
    // Move to map
    m_tessEvalShader[id] = std::move(shader);
    return true;
}

bool ARenderer::loadGeometryShader(ResourceId id, IResourceManager* resourceManager)
{
    // Unused id
    if (id == -1)
    {
        return true;
    }
    // Already loaded
    if (m_geometryShader.count(id) != 0)
    {
        return true;
    }
    // Load from resoucre manager
    std::string text;
    resourceManager->getString(id, text);
    if (text.empty())
    {
        return false;
    }
    std::unique_ptr<TShaderObject<GL_GEOMETRY_SHADER>> shader(
        new TShaderObject<GL_GEOMETRY_SHADER>(text));
    // Check compile error
    if (!shader->isValid())
    {
        LOG_ERROR("%s", shader->getErrorString().c_str());
        return false;
    }
    // Move to map
    m_geometryShader[id] = std::move(shader);
    return true;
}

bool ARenderer::loadFragmentShader(ResourceId id, IResourceManager* resourceManager)
{
    // Unused id
    if (id == -1)
    {
        return true;
    }
    // Already loaded
    if (m_fragmentShader.count(id) != 0)
    {
        return true;
    }
    // Load from resoucre manager
    std::string text;
    resourceManager->getString(id, text);
    if (text.empty())
    {
        return false;
    }
    std::unique_ptr<TShaderObject<GL_FRAGMENT_SHADER>> shader(
        new TShaderObject<GL_FRAGMENT_SHADER>(text));
    // Check compile error
    if (!shader->isValid())
    {
        LOG_ERROR("%s", shader->getErrorString().c_str());
        return false;
    }
    // Move to map
    m_fragmentShader[id] = std::move(shader);
    return true;
}

void ARenderer::handleImageEvent(ResourceId id, EListenerEvent event,
                                 IResourceManager* resourceManager)
{
    std::vector<unsigned char> data;
    unsigned int width;
    unsigned int height;
    EColorFormat format;

    switch (event)
    {
    case EListenerEvent::Create:
        assert(m_textures.count(id) == 0 && "Texture id already exists");

        if (!resourceManager->getImage(id, data, width, height, format))
        {
            assert(false && "Failed to access image resource");
        }
        // Create new texture
        m_textures[id] =
            std::move(std::unique_ptr<CTexture>(new CTexture(data, width, height, format)));
        break;

    case EListenerEvent::Change:
        assert(m_textures.count(id) == 1 && "Texture id does not exist");

        if (!resourceManager->getImage(id, data, width, height, format))
        {
            assert(false && "Failed to access image resource");
        }
        // Reinitialize texture on change
        m_textures.at(id)->init(data, width, height, format);
        break;

    case EListenerEvent::Delete:
        // Keep texture?
        break;

    default:
        break;
    }
}

void ARenderer::handleMeshEvent(ResourceId id, EListenerEvent event,
                                IResourceManager* resourceManager)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    EPrimitiveType type;

    switch (event)
    {
    case EListenerEvent::Create:
        assert(m_meshes.count(id) == 0 && "Mesh id already exists");

        if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
        {
            assert(false && "Failed to access mesh resource");
        }
        // Create new texture
        m_meshes[id] =
            std::move(std::unique_ptr<CMesh>(new CMesh(vertices, indices, normals, uvs, type)));
        break;

    case EListenerEvent::Change:
        assert(m_meshes.count(id) == 1 && "Mesh id does not exist");

        if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
        {
            assert(false && "Failed to access mesh resource");
        }
        // Reinitialize mesh on change
        m_meshes.at(id)->init(vertices, indices, normals, uvs, type);
        break;

    case EListenerEvent::Delete:
        // Keep mesh?
        break;

    default:
        break;
    }
}

void ARenderer::handleMaterialEvent(ResourceId id, EListenerEvent event,
                                    IResourceManager* resourceManager)
{
    ResourceId diffuse;
    ResourceId normal;
    ResourceId specular;
    ResourceId glow;
    ResourceId alpha;
    ResourceId customShader;

    switch (event)
    {
    case EListenerEvent::Create:
        assert(m_materials.count(id) == 0 && "Material id already exists");

        if (!resourceManager->getMaterial(id, diffuse, normal, specular, glow, alpha, customShader))
        {
            assert(false && "Failed to access material resource");
        }

        // Create new material
        m_materials[id] = std::move(std::unique_ptr<CMaterial>(
            new CMaterial(getTexture(diffuse), getTexture(normal), getTexture(specular),
                          getTexture(glow), getTexture(alpha), getShaderProgram(customShader))));
        break;

    case EListenerEvent::Change:
        assert(m_materials.count(id) == 1 && "Material id does not exist");

        if (!resourceManager->getMaterial(id, diffuse, normal, specular, glow, alpha, customShader))
        {
            assert(false && "Failed to access material resource");
        }

        // Reinitialize material on change
        m_materials.at(id)->init(getTexture(diffuse), getTexture(normal), getTexture(specular),
                                 getTexture(glow), getTexture(alpha),
                                 getShaderProgram(customShader));
        break;

    case EListenerEvent::Delete:
        // Keep material?
        break;

    default:
        break;
    }
}

void ARenderer::handleShaderEvent(ResourceId id, EListenerEvent event,
                                  IResourceManager* resourceManager)
{
    ResourceId vertex;
    ResourceId tessControl;
    ResourceId tessEval;
    ResourceId geometry;
    ResourceId fragment;

    switch (event)
    {
    case EListenerEvent::Create:
        // TODO Replace assert with log statement and global error handler
        assert(m_shaderPrograms.count(id) == 0 && "Shader id already exists");

        // Load shader source ids
        if (!resourceManager->getShader(id, vertex, tessControl, tessEval, geometry, fragment))
        {
            // TODO Replace assert with log statement and global error handler
            assert(false && "Failed to access shader resource");
        }

        // Load shader objects from source
        if (!loadVertexShader(vertex, resourceManager) ||
            !loadTessControlShader(tessControl, resourceManager) ||
            !loadTessEvalShader(tessEval, resourceManager) ||
            !loadGeometryShader(geometry, resourceManager) ||
            !loadFragmentShader(fragment, resourceManager))
        {
            // TODO Log error
            return;
        }

        // Add create shader program
        m_shaderPrograms[id] = std::move(std::unique_ptr<CShaderProgram>(new CShaderProgram(
            getVertexShaderObject(vertex), getTessControlShaderObject(tessControl),
            getTessEvalShaderObject(tessEval), getGeometryShaderObject(geometry),
            getFragmentShaderObject(fragment))));

        break;

    case EListenerEvent::Change:
    // TODO Implement

    case EListenerEvent::Delete:
        // TODO Keep shader?
        break;

    default:
        // TODO Log error on unknown event?
        break;
    }
}

void ARenderer::handleStringEvent(ResourceId id, EListenerEvent event,
                                  IResourceManager* resourceManager)
{
    // Does not need processing, shader events handle source loading
}
