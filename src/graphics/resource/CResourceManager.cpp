#include "CResourceManager.h"

#include <fstream>
#include <sstream>

#include "lodepng.h"
#include "tiny_obj_loader.h"

#include "graphics/IResourceListener.h"

#include "io/CIniFile.h"
#include "io/CObjModelLoader.h"

#include "debug/Log.h"

CResourceManager::CResourceManager()
    : m_nextMeshId(0), m_nextImageId(0), m_nextMaterialId(0), m_nextStringId(0), m_nextShaderId(0)
{
    return;
}

ResourceId CResourceManager::createMesh(const std::vector<float>& vertices,
                                        const std::vector<unsigned int>& indices,
                                        const std::vector<float>& normals,
                                        const std::vector<float>& uvs, EPrimitiveType type)
{
    // Create mesh id
    ResourceId id = m_nextMeshId;
    ++m_nextMeshId;

    // Add mesh
    m_meshes[id] = SMesh(vertices, indices, normals, uvs, type);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Mesh, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::loadMesh(const std::string& file)
{
    auto entry = m_meshFiles.find(file);
    if (entry != m_meshFiles.end())
    {
        return entry->second;
    }

    // Retrieve file extension
    auto pos = file.find_last_of('.');
    if (pos == std::string::npos)
    {
        LOG_ERROR("The mesh file %s does not have a file extension and could not be loaded.",
                  file.c_str());
        return -1;
    }
    std::string extension = file.substr(file.find_last_of('.') + 1);

	// Mesh resource id
	ResourceId meshId = -1;

	// Decide loading method based on extension
	// TODO Register loader functions for extensions
    if (extension == "obj")
	{
		// Wavefront OBJ file format loaded with tinyobj
        std::vector<tinyobj::shape_t> shapes;
        // Load as obj
        std::string err = tinyobj::LoadObj(shapes, file.c_str());

        if (!err.empty())
        {
            LOG_ERROR("%s.", err.c_str());
            return -1;
        }
        if (shapes.size() > 1)
        {
            LOG_WARNING("Multple meshes in obj files not supported.");
        }
        if (shapes.empty())
        {
            LOG_ERROR("No mesh data loaded from file %s.", file.c_str());
            return -1;
        }
        // Create mesh resource
        meshId = createMesh(shapes.at(0).mesh.positions, shapes.at(0).mesh.indices,
                                       shapes.at(0).mesh.normals, shapes.at(0).mesh.texcoords,
                                       EPrimitiveType::Triangle);
    }
	else if (extension == "oni")
	{
		// Load without building index buffer
		CObjModelLoader objLoader;
		if (!objLoader.load(file))
		{
			LOG_ERROR("Failed to load mesh file %s as non-indexed obj file.", file.c_str());
			return -1;
		}
		meshId = createMesh(objLoader.getVertices(), {}, objLoader.getNormals(), objLoader.getUV(), EPrimitiveType::Triangle);
	}

    if (meshId == -1)
    {
        LOG_ERROR("Failed to create mesh resource id from file %s.", file.c_str());
        return -1;
    }
    m_meshFiles[file] = meshId;
    return meshId;
}

bool CResourceManager::getMesh(ResourceId id, std::vector<float>& vertices,
                               std::vector<unsigned int>& indices, std::vector<float>& normals,
                               std::vector<float>& uvs, EPrimitiveType& type) const
{
    // Retrieve from map
    auto iter = m_meshes.find(id);
    if (iter == m_meshes.end())
    {
        return false;
    }
    // Copy data
    vertices = iter->second.m_vertices;
    indices = iter->second.m_indices;
    normals = iter->second.m_normals;
    uvs = iter->second.m_uvs;
    type = iter->second.m_type;
    return true;
}

ResourceId CResourceManager::createImage(const std::vector<unsigned char>& imageData,
                                         unsigned int width, unsigned int height,
                                         EColorFormat format)
{
    // Create image
    ResourceId id = m_nextImageId;
    ++m_nextImageId;

    // TODO Sanity check if mesh id exists?
    // Add mesh
    m_images[id] = SImage(imageData, width, height, format);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Image, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::loadImage(const std::string& file, EColorFormat format)
{
    auto entry = m_imageFiles.find(file);
    if (entry != m_imageFiles.end())
    {
        return entry->second;
    }

    // TODO Check extension, png format assumed
    std::vector<unsigned char> data;
    unsigned int width;
    unsigned int height;

    // Map color type
    LodePNGColorType colorType;
    switch (format)
    {
    case EColorFormat::GreyScale8:
        colorType = LCT_GREY;
        break;
    case EColorFormat::RGB24:
        colorType = LCT_RGB;
        break;
    case EColorFormat::RGBA32:
        colorType = LCT_RGBA;
        break;
    default:
        LOG_ERROR("Unknown color format encountered while loading image file %s.", file.c_str());
        return -1;
        break;
    }

    // Decode image data
    unsigned int err = lodepng::decode(data, width, height, file, colorType);
    if (err != 0)
    {
        LOG_ERROR("An error occured while decoding the image file %s: %s", file.c_str(),
                  lodepng_error_text(err));
        return -1;
    }

    ResourceId imageId = createImage(data, width, height, format);
    if (imageId == -1)
    {
        LOG_ERROR("Failed to create image resource id from file %s.", file.c_str());
        return -1;
    }
    m_imageFiles[file] = imageId;
    return imageId;
}

bool CResourceManager::getImage(ResourceId id, std::vector<unsigned char>& data,
                                unsigned int& width, unsigned int& height,
                                EColorFormat& format) const
{
    // Retrieve from map
    auto iter = m_images.find(id);
    if (iter == m_images.end())
    {
        return false;
    }
    // Copy data
    data = iter->second.m_data;
    width = iter->second.m_width;
    height = iter->second.m_height;
    format = iter->second.m_format;
    return true;
}

ResourceId CResourceManager::createMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal,
                                            ResourceId specular, ResourceId glow,
                                            ResourceId customShader)
{
    // Create material
    ResourceId id = m_nextMaterialId;
    ++m_nextMaterialId;

    // Add material
    m_materials[id] = SMaterial(diffuse, alpha, normal, specular, glow, customShader);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Material, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::loadMaterial(const std::string& file)
{
    auto entry = m_materialFiles.find(file);
    if (entry != m_materialFiles.end())
    {
        return entry->second;
    }

    CIniFile ini;
    if (!ini.load(file))
    {
        LOG_ERROR("Failed to load material file %s as ini file.", file.c_str());
        return -1;
    }

    ResourceId diffuseId = -1;
    if (ini.hasKey("diffuse", "file"))
    {
        // Diffuse texture is RGB format, ignore alpha
        diffuseId = loadImage(ini.getValue("diffuse", "file", "error"), EColorFormat::RGB24);
        if (diffuseId == -1)
        {
            LOG_ERROR("Failed to load diffuse texture specified in material file %s.",
                      file.c_str());
            return -1;
        }
    }

    ResourceId alphaId = -1;
    if (ini.hasKey("alpha", "file"))
    {
        // Alpha texture is grey-scale format
        alphaId = loadImage(ini.getValue("alpha", "file", "error"), EColorFormat::GreyScale8);
        if (alphaId == -1)
        {
            LOG_ERROR("Failed to load alpha texture specified in material file %s.", file.c_str());
            return -1;
        }
    }

    ResourceId normalId = -1;
    if (ini.hasKey("normal", "file"))
    {
        // Normal texture is RGB format
        normalId = loadImage(ini.getValue("normal", "file", "error"), EColorFormat::RGB24);
        if (normalId == -1)
        {
            LOG_ERROR("Failed to load normal texture specified in material file %s.", file.c_str());
            return -1;
        }
    }

    ResourceId specularId = -1;
    if (ini.hasKey("specular", "file"))
    {
        // Specular texture is grey-scale format
        specularId = loadImage(ini.getValue("specular", "file", "error"), EColorFormat::GreyScale8);
        if (specularId == -1)
        {
            LOG_ERROR("Failed to load specular texture specified in material file %s.",
                      file.c_str());
            return -1;
        }
    }

    ResourceId glowId = -1;
    if (ini.hasKey("glow", "file"))
    {
        // Glow texture is grey-scale format
        glowId = loadImage(ini.getValue("glow", "file", "error"), EColorFormat::GreyScale8);
        if (glowId == -1)
        {
            LOG_ERROR("Failed to load glow texture specified in material file %s.", file.c_str());
            return -1;
        }
    }

    ResourceId customShaderId = -1;
    if (ini.hasKey("shader", "file"))
    {
        // Has custom shader file specified
        customShaderId = loadShader(ini.getValue("shader", "file", "error"));
        if (customShaderId == -1)
        {
            LOG_ERROR("Failed to load custom shader file specified in material file %s.",
                      file.c_str());
            return -1;
        }
    }

    ResourceId materialId =
        createMaterial(diffuseId, alphaId, normalId, specularId, glowId, customShaderId);
    if (materialId == -1)
    {
        LOG_ERROR("Failed to create material resource id for material file %s.", file.c_str());
        return -1;
    }
    m_materialFiles[file] = materialId;
    return materialId;
}

bool CResourceManager::getMaterial(ResourceId id, ResourceId& diffuse, ResourceId& alpha,
                                   ResourceId& normal, ResourceId& specular, ResourceId& glow,
                                   ResourceId& customShader) const
{
    // Retrieve from map
    auto iter = m_materials.find(id);
    if (iter == m_materials.end())
    {
        return false;
    }
    // Copy data
    diffuse = iter->second.m_diffuse;
    alpha = iter->second.m_alpha;
    normal = iter->second.m_normal;
    specular = iter->second.m_specular;
    glow = iter->second.m_glow;
    customShader = iter->second.m_customShader;
    return true;
}

ResourceId CResourceManager::createString(const std::string& text)
{
    // Create string
    ResourceId id = m_nextStringId;
    ++m_nextStringId;

    // Add string
    m_strings[id] = text;

    // Notify listener with create event
    notifyResourceListeners(EResourceType::String, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::loadString(const std::string& file)
{
    auto iter = m_textFiles.find(file);
    if (iter != m_textFiles.end())
    {
        return iter->second;
    }

    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        LOG_ERROR("Failed to open the text file %s.", file.c_str());
        return -1;
    }

    // Load file
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    ResourceId stringId = -1;

    // Create new string entry
    stringId = createString(text);
    if (stringId == -1)
    {
        LOG_ERROR("Failed to create string id for text file %s.", file.c_str());
        return -1;
    }

    m_textFiles[file] = stringId;
    return stringId;
}

bool CResourceManager::getString(ResourceId id, std::string& text) const
{
    // Retrieve from map
    auto iter = m_strings.find(id);
    if (iter == m_strings.end())
    {
        return false;
    }
    // Copy data
    text = iter->second;
    return true;
}

ResourceId CResourceManager::createShader(ResourceId vertex, ResourceId tessCtrl,
                                          ResourceId tessEval, ResourceId geometry,
                                          ResourceId fragment)
{
    // Create shader
    ResourceId id = m_nextShaderId;
    ++m_nextShaderId;

    // Add shader
    m_shaders[id] = SShader(vertex, tessCtrl, tessEval, geometry, fragment);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Shader, id, EListenerEvent::Create);
    return id;
}

bool CResourceManager::getShader(ResourceId id, ResourceId& vertex, ResourceId& tessCtrl,
                                 ResourceId& tessEval, ResourceId& geometry,
                                 ResourceId& fragment) const
{
    // Retrieve from map
    auto iter = m_shaders.find(id);
    if (iter == m_shaders.end())
    {
        return false;
    }
    // Copy data
    vertex = iter->second.m_vertex;
    tessCtrl = iter->second.m_tessCtrl;
    tessEval = iter->second.m_tessEval;
    geometry = iter->second.m_geometry;
    fragment = iter->second.m_fragment;
    return true;
}

ResourceId CResourceManager::loadShader(const std::string& file)
{
    // Check if shader exists
    auto entry = m_shaderFiles.find(file);
    if (entry != m_shaderFiles.end())
    {
        return entry->second;
    }

    // Load shader ini
    CIniFile ini;
    if (!ini.load(file))
    {
        LOG_ERROR("Failed to load shader program file %s", file.c_str());
        return -1;
    }

    // Requires vertex and fragment shader files
    if (!ini.hasKey("vertex", "file") || !ini.hasKey("fragment", "file"))
    {
        LOG_ERROR(
            "The shader program file %s is missing a vertex and/or fragment shader source file "
            "specifier.",
            file.c_str());
        return -1;
    }

    ResourceId vertexId = loadString(ini.getValue("vertex", "file", "error"));
    if (vertexId == -1)
    {
        LOG_ERROR(
            "The vertex shader source file %s, specified in the shader program file %s could not "
            "be loaded.",
            ini.getValue("vertex", "file", "error").c_str(), file.c_str());
        return -1;
    }

    ResourceId fragmentId = loadString(ini.getValue("fragment", "file", "error"));
    if (fragmentId == -1)
    {
        LOG_ERROR(
            "The fragment shader source file %s, specified in the shader program file %s could not "
            "be loaded.",
            ini.getValue("fragment", "file", "error").c_str(), file.c_str());
        return -1;
    }

    ResourceId tessCtrlId = -1;
    // Check for and load tessellation control shader source
    if (ini.hasKey("tessellation_control", "file"))
    {
        tessCtrlId = loadString(ini.getValue("tessellation_control", "file", "error"));
        if (tessCtrlId == -1)
        {
            LOG_ERROR(
                "The tessellation control shader source file %s, specified in the shader program "
                "file %s could not be loaded.",
                ini.getValue("tessellation_control", "file", "error").c_str(), file.c_str());
            return -1;
        }
    }

    ResourceId tessEvalId = -1;
    // Check for and load tessellation evaluation shader source
    if (ini.hasKey("tessellation_evaluation", "file"))
    {
        tessEvalId = loadString(ini.getValue("tessellation_evaluation", "file", "error"));
        if (tessEvalId == -1)
        {
            LOG_ERROR(
                "The tessellation evaluation shader source file %s, specified in the shader "
                "program file %s could not be loaded.",
                ini.getValue("tessellation_evaluation", "file", "error").c_str(), file.c_str());
            return -1;
        }
    }

    ResourceId geometryId = -1;
    // Check for and load geometry shader source
    if (ini.hasKey("geometry", "file"))
    {
        geometryId = loadString(ini.getValue("geometry", "file", "error"));
        if (geometryId == -1)
        {
            LOG_ERROR(
                "The geometry shader source file %s, specified in the shader program file %s could "
                "not be loaded.",
                ini.getValue("geometry", "file", "error").c_str(), file.c_str());
            return -1;
        }
    }

    ResourceId shaderId = -1;
    // Create new entry
    shaderId = createShader(vertexId, tessCtrlId, tessEvalId, geometryId, fragmentId);
    if (shaderId == -1)
    {
        LOG_ERROR("Failed to create reasource id for shader file %s.", file.c_str());
        return -1;
    }
    m_shaderFiles[file] = shaderId;
    return shaderId;
}

void CResourceManager::addResourceListener(IResourceListener* listener)
{
    m_resourceListeners.push_back(listener);
    listener->onAttach(this);
}

void CResourceManager::removeResourceListener(IResourceListener* listener)
{
    m_resourceListeners.remove(listener);
    listener->onDetach(this);
}

void CResourceManager::notifyResourceListeners(EResourceType type, ResourceId id,
                                               EListenerEvent event)
{
    for (const auto& listener : m_resourceListeners)
    {
        listener->notify(type, id, event, this);
    }
}
