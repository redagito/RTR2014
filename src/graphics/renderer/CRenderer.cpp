#include "CRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext.hpp>

#include "debug/Log.h"

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IResourceManager.h"

#include "debug/RendererDebug.h"

CRenderer::CRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
	: m_resourceManager(resourceManager), m_gBuffer(nullptr)
{
    // Add resource listener
    m_resourceManager->addResourceListener(this);

	// Init default shaders
	initDefaultShaders();

	// Init GBuffer for deferred rendering
	initFrameBuffer();

    // Set clear color
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	// Depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Backface culling disabled for debugging
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Winding order, standard is counter-clockwise
	glFrontFace(GL_CCW);

	// Error check
	std::string error;
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}
    return;
}

CRenderer::~CRenderer()
{
    // Remove resource listener
    m_resourceManager->removeResourceListener(this);
}

void CRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{
    // Draw init
    window.setActive();

    // Initializiation
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

	// TODO Default shader for rendering, remove later
	m_defaultShader->setUniform("view", camera.getView());
	m_defaultShader->setUniform("projection", camera.getProjection());

    // TODO Implement multi pass system

    // Query visible scene objects
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Traverse visible objects
    while (query->hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query->getNextObject();

        // Object attributes
        ResourceId mesh = -1;
        ResourceId material = -1;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        // Retrieve object data
        if (!scene.getObject(id, mesh, material, position, rotation, scale))
        {
            // Invalid id
            LOG_ERROR("Invalid scene object id %l.", id);
        }
        else
        {
            // Forward draw call, stores render requests with custom shader set in material
            draw(mesh, position, rotation, scale, material);
        }
    }

	// Draw objects with custom shader set
	for (auto& request : m_customShaderMeshes)
	{
		draw(request.m_mesh, request.m_translation, request.m_rotation, request.m_scale, request.m_material);
	}
	m_customShaderMeshes.clear();

    // Post draw error check
	std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }
}

void CRenderer::draw(ResourceId meshId, const glm::vec3& position, const glm::vec3& rotation,
                     const glm::vec3& scale, ResourceId materialId)
{
    // Resolve ids
    CMesh* mesh = getMesh(meshId);
    CMaterial* material = getMaterial(materialId);

    // Create matrices
    glm::mat4 translationMatrix = glm::translate(position);
    // TODO Rotation calculation is probably wrong
	glm::mat4 rotationMatrix = glm::mat4(1.f); // glm::rotate(1.f, rotation);
    glm::mat4 scaleMatrix = glm::scale(scale);

	// Defer rendering for materials with custom shaders
	if (material->hasCustomShader())
	{
		m_customShaderMeshes.push_back(SRenderRequest(mesh, material, translationMatrix, rotationMatrix, scaleMatrix));
		return;
	}
    // Forward draw call
    draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material);
}

void CRenderer::draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                     const glm::mat4& scale, CMaterial* material)
{
    // TODO Rendering logic
    // TODO Set default builtin shader
	CShaderProgram* shader = m_defaultShader;
	shader->setUniform("rotation", rotation);
	shader->setUniform("translation", translation);
	shader->setUniform("scale", scale);
	shader->setUniform("model", translation * rotation * scale);

    shader->setActive();

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
	mesh->getVertexArray()->setActive();

    // Set draw mode
    GLenum mode;
    unsigned int primitiveSize = 0;
    switch (mesh->getPrimitiveType())
    {
    case EPrimitiveType::Point:
        mode = GL_POINTS;
        primitiveSize = 1;
        break;
    case EPrimitiveType::Line:
        mode = GL_LINE;
        primitiveSize = 2;
        break;
    case EPrimitiveType::Triangle:
        mode = GL_TRIANGLES;
        primitiveSize = 3;
        break;
    default:
        LOG_ERROR("Invalid primitive type");
        return;
    }

    // Decide on draw method based on the stored data
    if (mesh->hasIndexBuffer())
    {
        // Indexed draw, probably faster
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

void CRenderer::onAttach(IResourceManager* resourceManager)
{
    // TODO Implement
    return;
}

void CRenderer::onDetach(IResourceManager* resourceManager)
{
    // TODO Implement
    return;
}

void CRenderer::notify(EResourceType type, ResourceId id, EListenerEvent event,
                       IResourceManager* resourceManager)
{
    // Handle events
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

CMesh* CRenderer::getMesh(ResourceId id) const
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

CMaterial* CRenderer::getMaterial(ResourceId id) const
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

CTexture* CRenderer::getTexture(ResourceId id) const
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

CShaderProgram* CRenderer::getShaderProgram(ResourceId id) const
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

TShaderObject<GL_VERTEX_SHADER>* CRenderer::getVertexShaderObject(ResourceId id) const
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

TShaderObject<GL_TESS_CONTROL_SHADER>* CRenderer::getTessControlShaderObject(ResourceId id) const
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

TShaderObject<GL_TESS_EVALUATION_SHADER>* CRenderer::getTessEvalShaderObject(ResourceId id) const
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

TShaderObject<GL_GEOMETRY_SHADER>* CRenderer::getGeometryShaderObject(ResourceId id) const
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

TShaderObject<GL_FRAGMENT_SHADER>* CRenderer::getFragmentShaderObject(ResourceId id) const
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

bool CRenderer::loadVertexShader(ResourceId id, IResourceManager* resourceManager)
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

bool CRenderer::loadTessControlShader(ResourceId id, IResourceManager* resourceManager)
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

bool CRenderer::loadTessEvalShader(ResourceId id, IResourceManager* resourceManager)
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

bool CRenderer::loadGeometryShader(ResourceId id, IResourceManager* resourceManager)
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

bool CRenderer::loadFragmentShader(ResourceId id, IResourceManager* resourceManager)
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

void CRenderer::handleImageEvent(ResourceId id, EListenerEvent event,
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

void CRenderer::handleMeshEvent(ResourceId id, EListenerEvent event,
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

void CRenderer::handleMaterialEvent(ResourceId id, EListenerEvent event,
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

void CRenderer::handleShaderEvent(ResourceId id, EListenerEvent event,
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

void CRenderer::handleStringEvent(ResourceId id, EListenerEvent event,
                                  IResourceManager* resourceManager)
{
    // Does not need processing, shader events handel source loading
}

void CRenderer::initDefaultShaders()
{
	ResourceId shaderId = m_resourceManager->loadShader("data/shader/shader_test_0.ini");
	m_defaultShader = m_shaderPrograms.at(shaderId).get();
}

void CRenderer::initFrameBuffer()
{

}