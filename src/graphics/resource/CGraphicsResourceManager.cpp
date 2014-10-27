#include "CGraphicsResourceManager.h"

#include <cassert>
#include <string>

#include "resource/IResourceManager.h"
#include "debug/Log.h"

CGraphicsResourceManager::CGraphicsResourceManager()
{
	// Create default textures
	initDefaultTextures();
	return;
}

CGraphicsResourceManager::~CGraphicsResourceManager()
{
	// Remove resource listener
	while (!m_registeredManagers.empty())
	{
		// Calls onDettach, which removes from ist
		m_registeredManagers.front()->removeResourceListener(this);
	}
}

void CGraphicsResourceManager::onAttach(IResourceManager* resourceManager)
{
	// Add to known resource managers
	if (std::find(m_registeredManagers.begin(), m_registeredManagers.end(), resourceManager) == m_registeredManagers.end())
	{
		m_registeredManagers.push_back(resourceManager);
	}
	return;
}

void CGraphicsResourceManager::onDetach(IResourceManager* resourceManager)
{
	m_registeredManagers.remove(resourceManager);
	return;
}

void CGraphicsResourceManager::notify(EResourceType type, ResourceId id, EListenerEvent event,
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
		LOG_ERROR("Unknown resource type encountered.");
		break;
	}
}

void CGraphicsResourceManager::initDefaultTextures()
{
	// Default diffuse texture is deep pink to signal errors/missing textures
	m_defaultDiffuseTexture.reset(new CTexture({ 238, 18, 137 }, 1, 1, EColorFormat::RGB24, 0));

	// Default normal texture with straight/non-perturbed normals
	// Discussion here:
	// http://www.gameartisans.org/forums/threads/1985-Normal-Map-RGB-127-127-255-or-128-128-255
	m_defaultNormalTexture.reset(new CTexture({ 128, 128, 255 }, 1, 1, EColorFormat::RGB24, 0));

	// Default specular texture is black (no specular highlights)
	m_defaultSpecularTexture.reset(new CTexture({ 0 }, 1, 1, EColorFormat::GreyScale8));

	// Default glow texture is black (no glow)
	m_defaultGlowTexture.reset(new CTexture({ 0 }, 1, 1, EColorFormat::GreyScale8));

	// Default alpha texture is white (completely opaque)
	m_defaultAlphaTexture.reset(new CTexture({ 255 }, 1, 1, EColorFormat::GreyScale8));
}

CMesh* CGraphicsResourceManager::getMesh(ResourceId id) const
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

CMaterial* CGraphicsResourceManager::getMaterial(ResourceId id) const
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

CTexture* CGraphicsResourceManager::getTexture(ResourceId id) const
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

CShaderProgram* CGraphicsResourceManager::getShaderProgram(ResourceId id) const
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


CTexture* CGraphicsResourceManager::getDefaultDiffuseTexture() const
{
	return m_defaultDiffuseTexture.get();
}

CTexture* CGraphicsResourceManager::getDefaultNormalTexture() const
{
	return m_defaultNormalTexture.get();
}

CTexture* CGraphicsResourceManager::getDefaultSpecularTexture() const
{
	return m_defaultSpecularTexture.get();
}

CTexture* CGraphicsResourceManager::getDefaultGlowTexture() const
{
	return m_defaultGlowTexture.get();
}

CTexture* CGraphicsResourceManager::getDefaultAlphaTexture() const
{
	return m_defaultAlphaTexture.get();
}

TShaderObject<GL_VERTEX_SHADER>* CGraphicsResourceManager::getVertexShaderObject(ResourceId id) const
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

TShaderObject<GL_TESS_CONTROL_SHADER>* CGraphicsResourceManager::getTessControlShaderObject(ResourceId id) const
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

TShaderObject<GL_TESS_EVALUATION_SHADER>* CGraphicsResourceManager::getTessEvalShaderObject(ResourceId id) const
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

TShaderObject<GL_GEOMETRY_SHADER>* CGraphicsResourceManager::getGeometryShaderObject(ResourceId id) const
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

TShaderObject<GL_FRAGMENT_SHADER>* CGraphicsResourceManager::getFragmentShaderObject(ResourceId id) const
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

bool CGraphicsResourceManager::loadVertexShader(ResourceId id, IResourceManager* resourceManager)
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

bool CGraphicsResourceManager::loadTessControlShader(ResourceId id, IResourceManager* resourceManager)
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

bool CGraphicsResourceManager::loadTessEvalShader(ResourceId id, IResourceManager* resourceManager)
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

bool CGraphicsResourceManager::loadGeometryShader(ResourceId id, IResourceManager* resourceManager)
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

bool CGraphicsResourceManager::loadFragmentShader(ResourceId id, IResourceManager* resourceManager)
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

void CGraphicsResourceManager::handleImageEvent(ResourceId id, EListenerEvent event,
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

void CGraphicsResourceManager::handleMeshEvent(ResourceId id, EListenerEvent event,
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

void CGraphicsResourceManager::handleMaterialEvent(ResourceId id, EListenerEvent event,
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

void CGraphicsResourceManager::handleShaderEvent(ResourceId id, EListenerEvent event,
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

void CGraphicsResourceManager::handleStringEvent(ResourceId id, EListenerEvent event,
	IResourceManager* resourceManager)
{
	// Does not need processing, shader events handle source loading
}
