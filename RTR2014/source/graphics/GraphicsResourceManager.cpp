#include <spdlog/spdlog.h>
#include "graphics/GraphicsResourceManager.h"

#include <cassert>
#include <string>

#include "resource/IResourceManager.h"

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

	// Id must exist
	// TODO Allow mesh loading if not found?
	assert(iter != m_meshes.end());
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

	// Id must exist
	// TODO Allow material loading if not found?
	assert(iter != m_materials.end());
	return iter->second.get();
}

Texture* GraphicsResourceManager::getTexture(ResourceId id) const
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

ShaderProgram* GraphicsResourceManager::getShaderProgram(ResourceId id) const
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
	return iter->second.get();
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

ShaderObject<GL_TESS_CONTROL_SHADER>* GraphicsResourceManager::getTessControlShaderObject(ResourceId id) const
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

ShaderObject<GL_TESS_EVALUATION_SHADER>* GraphicsResourceManager::getTessEvalShaderObject(ResourceId id) const
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

ShaderObject<GL_GEOMETRY_SHADER>* GraphicsResourceManager::getGeometryShaderObject(ResourceId id) const
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

ShaderObject<GL_FRAGMENT_SHADER>* GraphicsResourceManager::getFragmentShaderObject(ResourceId id) const
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

bool GraphicsResourceManager::loadVertexShader(ResourceId id, IResourceManager* resourceManager)
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
	std::unique_ptr<ShaderObject<GL_VERTEX_SHADER>> shader(
		new ShaderObject<GL_VERTEX_SHADER>(text));
	// Check compile error
	if (!shader->isValid())
	{
		spdlog::error("{}", shader->getErrorString().c_str());
		return false;
	}
	// Move to map
	m_vertexShader[id] = std::move(shader);
	return true;
}

bool GraphicsResourceManager::loadTessControlShader(ResourceId id, IResourceManager* resourceManager)
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
	std::unique_ptr<ShaderObject<GL_TESS_CONTROL_SHADER>> shader(
		new ShaderObject<GL_TESS_CONTROL_SHADER>(text));
	// Check compile error
	if (!shader->isValid())
	{
		spdlog::error("{}", shader->getErrorString().c_str());
		return false;
	}
	// Move to map
	m_tessConstrolShader[id] = std::move(shader);
	return true;
}

bool GraphicsResourceManager::loadTessEvalShader(ResourceId id, IResourceManager* resourceManager)
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
	std::unique_ptr<ShaderObject<GL_TESS_EVALUATION_SHADER>> shader(
		new ShaderObject<GL_TESS_EVALUATION_SHADER>(text));
	// Check compile error
	if (!shader->isValid())
	{
		spdlog::error("{}", shader->getErrorString().c_str());
		return false;
	}
	// Move to map
	m_tessEvalShader[id] = std::move(shader);
	return true;
}

bool GraphicsResourceManager::loadGeometryShader(ResourceId id, IResourceManager* resourceManager)
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
	std::unique_ptr<ShaderObject<GL_GEOMETRY_SHADER>> shader(
		new ShaderObject<GL_GEOMETRY_SHADER>(text));
	// Check compile error
	if (!shader->isValid())
	{
		spdlog::error("{}", shader->getErrorString().c_str());
		return false;
	}
	// Move to map
	m_geometryShader[id] = std::move(shader);
	return true;
}

bool GraphicsResourceManager::loadFragmentShader(ResourceId id, IResourceManager* resourceManager)
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
	std::unique_ptr<ShaderObject<GL_FRAGMENT_SHADER>> shader(
		new ShaderObject<GL_FRAGMENT_SHADER>(text));
	// Check compile error
	if (!shader->isValid())
	{
		spdlog::error("{}", shader->getErrorString().c_str());
		return false;
	}
	// Move to map
	m_fragmentShader[id] = std::move(shader);
	return true;
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
		assert(m_textures.count(id) == 0 && "Texture id already exists");

		if (!resourceManager->getImage(id, data, width, height, format))
		{
			assert(false && "Failed to access image resource");
		}
		// Create new texture
		m_textures[id] =
			std::move(std::unique_ptr<Texture>(new Texture(data, width, height, format)));
		break;

	case ListenerEvent::Change:
		assert(m_textures.count(id) == 1 && "Texture id does not exist");

		if (!resourceManager->getImage(id, data, width, height, format))
		{
			assert(false && "Failed to access image resource");
		}
		// Reinitialize texture on change
		m_textures.at(id)->init(data, width, height, format);
		break;

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
	PrimitiveType type;

	switch (event)
	{
	case ListenerEvent::Create:
		assert(m_meshes.count(id) == 0 && "Mesh id already exists");

		if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
		{
			assert(false && "Failed to access mesh resource");
		}
		// Create new texture
		m_meshes[id] =
			std::move(std::unique_ptr<Mesh>(new Mesh(vertices, indices, normals, uvs, type)));
		break;

	case ListenerEvent::Change:
		assert(m_meshes.count(id) == 1 && "Mesh id does not exist");

		if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
		{
			assert(false && "Failed to access mesh resource");
		}
		// Reinitialize mesh on change
		m_meshes.at(id)->init(vertices, indices, normals, uvs, type);
		break;

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
	ResourceId diffuse;
	ResourceId normal;
	ResourceId specular;
	ResourceId glow;
	ResourceId alpha;
	ResourceId customShader;

	switch (event)
	{
	case ListenerEvent::Create:
		assert(m_materials.count(id) == 0 && "Material id already exists");

		if (!resourceManager->getMaterial(id, diffuse, normal, specular, glow, alpha, customShader))
		{
			assert(false && "Failed to access material resource");
		}

		// Create new material
		m_materials[id] = std::move(std::unique_ptr<Material>(
			new Material(getTexture(diffuse), getTexture(normal), getTexture(specular),
			getTexture(glow), getTexture(alpha), getShaderProgram(customShader))));
		break;

	case ListenerEvent::Change:
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
	ResourceId vertex;
	ResourceId tessControl;
	ResourceId tessEval;
	ResourceId geometry;
	ResourceId fragment;

	switch (event)
	{
	case ListenerEvent::Create:
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

{
			// Add create shader program
			auto program = std::unique_ptr<ShaderProgram>(new ShaderProgram(
			getVertexShaderObject(vertex), getTessControlShaderObject(tessControl),
			getTessEvalShaderObject(tessEval), getGeometryShaderObject(geometry),
			getFragmentShaderObject(fragment)));

		if (!program->isValid())
		{
			spdlog::error("Failed to link shader program with id {}.", id);
			return;
		}

		m_shaderPrograms[id] = std::move(program);
		}

		break;

	case ListenerEvent::Change:
		// TODO Implement

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
