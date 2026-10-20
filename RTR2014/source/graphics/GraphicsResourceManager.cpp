#include <spdlog/spdlog.h>
#include "graphics/GraphicsResourceManager.h"

#include <cassert>
#include <cstdlib>
#include <string>

#include "resource/IResourceManager.h"

namespace
{
/**
 * \brief Returns the name of a shader stage.
 */
const char* shaderStageName(GLenum stage)
{
    switch (stage)
    {
    case GL_VERTEX_SHADER:
        return "vertex";
    case GL_TESS_CONTROL_SHADER:
        return "tessellation control";
    case GL_TESS_EVALUATION_SHADER:
        return "tessellation evaluation";
    case GL_GEOMETRY_SHADER:
        return "geometry";
    case GL_FRAGMENT_SHADER:
        return "fragment";
    default:
        return "unknown";
    }
}

/**
 * \brief Compiles a shader stage from the source text of a string resource.
 */
template <GLenum Stage>
bool loadShaderStage(
	ResourceId id, IResourceManager* resourceManager,
	std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<Stage>>>& shaders)
{
	// Unused id
	if (id == invalidResource)
	{
		return true;
	}
	// Already loaded
	if (shaders.count(id) != 0)
	{
		return true;
	}
	// Load from resource manager
	std::string text;
	if (!resourceManager->getString(id, text))
	{
		spdlog::error("The source of the {} shader {} could not be retrieved.",
		              shaderStageName(Stage), id);
		return false;
	}
	if (text.empty())
	{
		spdlog::error("The source of the {} shader {} is empty.", shaderStageName(Stage), id);
		return false;
	}
	std::unique_ptr<ShaderObject<Stage>> shader(new ShaderObject<Stage>(text));
	// Check compile error
	if (!shader->isValid())
	{
		spdlog::error("Failed to compile the {} shader {}: {}", shaderStageName(Stage), id,
		              shader->getErrorString());
		return false;
	}
	// Move to map
	shaders[id] = std::move(shader);
	return true;
}

/**
* \brief Returns a compiled shader stage, or null if it was not loaded.
*/
template <GLenum Stage>
ShaderObject<Stage>* getShaderStage(
	const std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<Stage>>>& shaders, ResourceId id,
	const char* stageName)
{
	// Invalid id
	if (id == invalidResource)
	{
		return nullptr;
	}
	// Search for id
	auto iter = shaders.find(id);

	// TODO Allow loading the shader stage if not found?
	if (iter == shaders.end())
	{
		spdlog::error("The requested {} id {} has not been loaded.", stageName, id);
		return nullptr;
	}
	return iter->second.get();
}
} // namespace

GraphicsResourceManager::GraphicsResourceManager()
{
	// Create default textures
	initDefaultTextures();
	return;
}

GraphicsResourceManager::~GraphicsResourceManager()
{
	// Remove resource listener
	while (!m_registeredManagers.empty())
	{
		// Calls onDettach, which removes from ist
		m_registeredManagers.front()->removeResourceListener(this);
	}
}

void GraphicsResourceManager::onAttach(IResourceManager* resourceManager)
{
	// Add to known resource managers
	if (std::find(m_registeredManagers.begin(), m_registeredManagers.end(), resourceManager) == m_registeredManagers.end())
	{
		m_registeredManagers.push_back(resourceManager);
	}
	return;
}

void GraphicsResourceManager::onDetach(IResourceManager* resourceManager)
{
	m_registeredManagers.remove(resourceManager);
	return;
}

void GraphicsResourceManager::notify(ResourceType type, ResourceId id, ListenerEvent event,
	IResourceManager* resourceManager)
{
	// Resolve resource type
	switch (type)
	{
	case ResourceType::Image:
		handleImageEvent(id, event, resourceManager);
		break;
	case ResourceType::Material:
		handleMaterialEvent(id, event, resourceManager);
		break;
	case ResourceType::Mesh:
		handleMeshEvent(id, event, resourceManager);
		break;
	case ResourceType::Shader:
		handleShaderEvent(id, event, resourceManager);
		break;
	case ResourceType::String:
		handleStringEvent(id, event, resourceManager);
		break;
	case ResourceType::Sound:
		break;
	default:
		spdlog::error("Unknown resource type encountered.");
		break;
	}
}

void GraphicsResourceManager::initDefaultTextures()
{
	// Default diffuse texture is deep pink to signal errors/missing textures
	m_defaultDiffuseTexture.reset(new Texture({ 238, 18, 137 }, 1, 1, ColorFormat::RGB24, 0));

	// Default normal texture with straight/non-perturbed normals
	// Discussion here:
	// http://www.gameartisans.org/forums/threads/1985-Normal-Map-RGB-127-127-255-or-128-128-255
	m_defaultNormalTexture.reset(new Texture({ 128, 128, 255 }, 1, 1, ColorFormat::RGB24, 0));

	// Default specular texture is black (no specular highlights)
	m_defaultSpecularTexture.reset(new Texture({ 0 }, 1, 1, ColorFormat::GreyScale8));

	// Default glow texture is black (no glow)
	m_defaultGlowTexture.reset(new Texture({ 0 }, 1, 1, ColorFormat::GreyScale8));

	// Default alpha texture is white (completely opaque)
	m_defaultAlphaTexture.reset(new Texture({ 255 }, 1, 1, ColorFormat::GreyScale8));
}

Mesh* GraphicsResourceManager::getMesh(ResourceId id) const
{
	// Invalid id
	if (id == -1)
	{
		return nullptr;
	}
	// Search for id
	auto iter = m_meshes.find(id);

	// TODO Allow mesh loading if not found?
	if (iter == m_meshes.end())
	{
		spdlog::error("The requested mesh id {} has not been loaded.", id);
		return nullptr;
	}
	return iter->second.get();
}

Material* GraphicsResourceManager::getMaterial(ResourceId id) const
{
	// Invalid id
	if (id == -1)
	{
		return nullptr;
	}
	// Search for id
	auto iter = m_materials.find(id);

	// TODO Allow material loading if not found?
	if (iter == m_materials.end())
	{
		spdlog::error("The requested material id {} has not been loaded.", id);
		return nullptr;
	}
	return iter->second.get();
}

Texture* GraphicsResourceManager::getTexture(ResourceId id) const
{
	return findTexture(id).get();
}

std::shared_ptr<Texture> GraphicsResourceManager::findTexture(ResourceId id) const
{
	// Invalid id
	if (id == -1)
	{
		return nullptr;
	}
	// Search for id
	auto iter = m_textures.find(id);

	// TODO Allow texture loading if not found?
	if (iter == m_textures.end())
	{
		spdlog::error("The requested texture id {} has not been loaded.", id);
		return nullptr;
	}
	return iter->second;
}

ShaderProgram* GraphicsResourceManager::getShaderProgram(ResourceId id) const
{
	return findShaderProgram(id).get();
}

std::shared_ptr<ShaderProgram> GraphicsResourceManager::findShaderProgram(ResourceId id) const
{
	// Invalid id
	if (id == invalidResource)
	{
		return nullptr;
	}
	// Search for id
	auto iter = m_shaderPrograms.find(id);

	// Id must exist
	// TODO Allow shader loading if not found?
	if (iter == m_shaderPrograms.end())
	{
		spdlog::error("The requested shader program id {} has not been loaded.", id);
		return nullptr;
	}
	return iter->second;
}


Texture* GraphicsResourceManager::getDefaultDiffuseTexture() const
{
	return m_defaultDiffuseTexture.get();
}

Texture* GraphicsResourceManager::getDefaultNormalTexture() const
{
	return m_defaultNormalTexture.get();
}

Texture* GraphicsResourceManager::getDefaultSpecularTexture() const
{
	return m_defaultSpecularTexture.get();
}

Texture* GraphicsResourceManager::getDefaultGlowTexture() const
{
	return m_defaultGlowTexture.get();
}

Texture* GraphicsResourceManager::getDefaultAlphaTexture() const
{
	return m_defaultAlphaTexture.get();
}

ShaderObject<GL_VERTEX_SHADER>* GraphicsResourceManager::getVertexShaderObject(ResourceId id) const
{
	return getShaderStage<GL_VERTEX_SHADER>(m_vertexShader, id, "vertex shader");
}

ShaderObject<GL_TESS_CONTROL_SHADER>* GraphicsResourceManager::getTessControlShaderObject(ResourceId id) const
{
	return getShaderStage<GL_TESS_CONTROL_SHADER>(m_tessConstrolShader, id, "tessellation control shader");
}

ShaderObject<GL_TESS_EVALUATION_SHADER>* GraphicsResourceManager::getTessEvalShaderObject(ResourceId id) const
{
	return getShaderStage<GL_TESS_EVALUATION_SHADER>(m_tessEvalShader, id, "tessellation evaluation shader");
}

ShaderObject<GL_GEOMETRY_SHADER>* GraphicsResourceManager::getGeometryShaderObject(ResourceId id) const
{
	return getShaderStage<GL_GEOMETRY_SHADER>(m_geometryShader, id, "geometry shader");
}

ShaderObject<GL_FRAGMENT_SHADER>* GraphicsResourceManager::getFragmentShaderObject(ResourceId id) const
{
	return getShaderStage<GL_FRAGMENT_SHADER>(m_fragmentShader, id, "fragment shader");
}

void GraphicsResourceManager::handleImageEvent(ResourceId id, ListenerEvent event,
	IResourceManager* resourceManager)
{
	std::vector<unsigned char> data;
	unsigned int width;
	unsigned int height;
	ColorFormat format;

	switch (event)
	{
	case ListenerEvent::Create:
		if (m_textures.count(id) != 0)
		{
			// Replacing the texture would leave the materials that use it with
			// a dangling pointer.
			spdlog::error("The texture id {} already exists.", id);
			return;
		}

		if (!resourceManager->getImage(id, data, width, height, format))
		{
			spdlog::error("Failed to access image resource {}.", id);
			return;
		}
		// Create new texture
		m_textures[id] = std::make_shared<Texture>(data, width, height, format);
		break;

	case ListenerEvent::Change:
	{
		auto texture = m_textures.find(id);
		if (texture == m_textures.end())
		{
			spdlog::error("The texture id {} does not exist.", id);
			return;
		}

		if (!resourceManager->getImage(id, data, width, height, format))
		{
			spdlog::error("Failed to access image resource {}.", id);
			return;
		}
		// Reinitialize texture on change
		texture->second->init(data, width, height, format);
		break;
	}

	case ListenerEvent::Delete:
		// Keep texture?
		break;

	default:
		break;
	}
}

void GraphicsResourceManager::handleMeshEvent(ResourceId id, ListenerEvent event,
	IResourceManager* resourceManager)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<float> normals;
	std::vector<float> uvs;
	PrimitiveType type = PrimitiveType::Invalid;

	switch (event)
	{
	case ListenerEvent::Create:
		if (m_meshes.count(id) != 0)
		{
			// Replacing the mesh would leave the scene objects that use it with
			// a dangling pointer.
			spdlog::error("The mesh id {} already exists.", id);
			return;
		}

		if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
		{
			spdlog::error("Failed to access mesh resource {}.", id);
			return;
		}
		// Create new mesh
		m_meshes[id] = std::make_unique<Mesh>(vertices, indices, normals, uvs, type);
		break;

	case ListenerEvent::Change:
	{
		auto mesh = m_meshes.find(id);
		if (mesh == m_meshes.end())
		{
			spdlog::error("The mesh id {} does not exist.", id);
			return;
		}

		if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
		{
			spdlog::error("Failed to access mesh resource {}.", id);
			return;
		}
		// Reinitialize mesh on change
		mesh->second->init(vertices, indices, normals, uvs, type);
		break;
	}

	case ListenerEvent::Delete:
		// Keep mesh?
		break;

	default:
		break;
	}
}

void GraphicsResourceManager::handleMaterialEvent(ResourceId id, ListenerEvent event,
	IResourceManager* resourceManager)
{
	ResourceId diffuse = invalidResource;
	ResourceId normal = invalidResource;
	ResourceId specular = invalidResource;
	ResourceId glow = invalidResource;
	ResourceId alpha = invalidResource;
	ResourceId customShader = invalidResource;

	switch (event)
	{
	case ListenerEvent::Create:
		if (m_materials.count(id) != 0)
		{
			// Replacing the material would leave the scene objects that use it
			// with a dangling pointer.
			spdlog::error("The material id {} already exists.", id);
			return;
		}

		if (!resourceManager->getMaterial(id, diffuse, normal, specular, glow, alpha, customShader))
		{
			spdlog::error("Failed to access material resource {}.", id);
			return;
		}

		// Create new material
		m_materials[id] = std::make_unique<Material>(
			findTexture(diffuse), findTexture(normal), findTexture(specular), findTexture(glow),
			findTexture(alpha), findShaderProgram(customShader));
		break;

	case ListenerEvent::Change:
	{
		auto material = m_materials.find(id);
		if (material == m_materials.end())
		{
			spdlog::error("The material id {} does not exist.", id);
			return;
		}

		if (!resourceManager->getMaterial(id, diffuse, normal, specular, glow, alpha, customShader))
		{
			spdlog::error("Failed to access material resource {}.", id);
			return;
		}

		// Reinitialize material on change
		material->second->init(findTexture(diffuse), findTexture(normal), findTexture(specular),
			findTexture(glow), findTexture(alpha),
			findShaderProgram(customShader));
		break;
	}

	case ListenerEvent::Delete:
		// Keep material?
		break;

	default:
		break;
	}
}

void GraphicsResourceManager::handleShaderEvent(ResourceId id, ListenerEvent event,
	IResourceManager* resourceManager)
{
	ResourceId vertex = invalidResource;
	ResourceId tessControl = invalidResource;
	ResourceId tessEval = invalidResource;
	ResourceId geometry = invalidResource;
	ResourceId fragment = invalidResource;

	switch (event)
	{
	case ListenerEvent::Create:
		if (m_shaderPrograms.count(id) != 0)
		{
			// Replacing the program would leave the materials that use it with
			// a dangling pointer.
			spdlog::error("The shader program id {} already exists.", id);
			return;
		}

		// Load shader source ids
		if (!resourceManager->getShader(id, vertex, tessControl, tessEval, geometry, fragment))
		{
			spdlog::error("Failed to access shader resource {}.", id);
			return;
		}

	// Load shader objects from source
	if (!loadShaderStage<GL_VERTEX_SHADER>(vertex, resourceManager, m_vertexShader) ||
		!loadShaderStage<GL_TESS_CONTROL_SHADER>(tessControl, resourceManager,
												  m_tessConstrolShader) ||
		!loadShaderStage<GL_TESS_EVALUATION_SHADER>(tessEval, resourceManager, m_tessEvalShader) ||
		!loadShaderStage<GL_GEOMETRY_SHADER>(geometry, resourceManager, m_geometryShader) ||
		!loadShaderStage<GL_FRAGMENT_SHADER>(fragment, resourceManager, m_fragmentShader))
	{
		// A shader program without all its stages cannot be used, the renderer
		// would draw nothing without ever saying why.
		std::string file;
		if (resourceManager->getShaderFile(id, file))
		{
			spdlog::error("The shader program from file {} could not be built.", file);
		}
		else
		{
			spdlog::error("The shader program with id {} could not be built.", id);
		}
		std::abort();
	}

{
			// Add create shader program
			auto program = std::make_shared<ShaderProgram>(
			getVertexShaderObject(vertex), getTessControlShaderObject(tessControl),
			getTessEvalShaderObject(tessEval), getGeometryShaderObject(geometry),
			getFragmentShaderObject(fragment));

		if (!program->isValid())
		{
			spdlog::error("Failed to link the shader program with id {}.", id);
			std::abort();
		}

		m_shaderPrograms[id] = std::move(program);
		}

		break;

	case ListenerEvent::Change:
		// TODO Implement hot-reload; without it, keep the existing program.
		break;

	case ListenerEvent::Delete:
		// TODO Keep shader?
		break;

	default:
		// TODO Log error on unknown event?
		break;
	}
}

void GraphicsResourceManager::handleStringEvent(ResourceId id, ListenerEvent event,
	IResourceManager* resourceManager)
{
	// Does not need processing, shader events handle source loading
}
