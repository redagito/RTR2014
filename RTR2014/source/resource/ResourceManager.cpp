#include <spdlog/spdlog.h>
#include "resource/ResourceManager.h"

#include <atomic>
#include <fstream>
#include <set>
#include <sstream>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_obj_loader.h"

#include "resource/IResourceListener.h"

#include "io/IniFile.h"

namespace
{
// Texture slots of a material file and the color format they are stored in.
constexpr int kMaterialTextureSlots = 5;
const char* const kMaterialTextureSlotNames[kMaterialTextureSlots] = {"diffuse", "normal", "specular",
                                                                      "glow", "alpha"};
const ColorFormat kMaterialTextureFormats[kMaterialTextureSlots] = {
    ColorFormat::RGB24, ColorFormat::RGB24, ColorFormat::GreyScale8, ColorFormat::GreyScale8,
    ColorFormat::GreyScale8};
} // namespace


ResourceManager::ResourceManager()
    : m_nextMeshId(0), m_nextImageId(0), m_nextMaterialId(0), m_nextStringId(0), m_nextShaderId(0)
{
    return;
}

ResourceId ResourceManager::createMesh(const std::vector<float>& vertices,
                                        const std::vector<unsigned int>& indices,
                                        const std::vector<float>& normals,
                                        const std::vector<float>& uvs, PrimitiveType type)
{
    // Create mesh id
    ResourceId id = m_nextMeshId;
    ++m_nextMeshId;

    // Add mesh
    m_meshes[id] = MeshData(vertices, indices, normals, uvs, type);

    // Notify listener with create event
    notifyResourceListeners(ResourceType::Mesh, id, ListenerEvent::Create);
    return id;
}

ResourceId ResourceManager::loadMesh(const std::string& file)
{
    auto entry = m_meshFiles.find(file);
    if (entry != m_meshFiles.end())
    {
        return entry->second;
    }

    // Publish a preloaded mesh
    auto pending = m_pendingMeshes.find(file);
    if (pending != m_pendingMeshes.end())
    {
        const MeshData& data = pending->second;
        ResourceId meshId = createMesh(data.m_vertices, data.m_indices, data.m_normals, data.m_uvs,
                                      data.m_type);
        m_pendingMeshes.erase(pending);
        if (meshId == -1)
        {
            spdlog::error("Failed to create mesh resource id from file {}.", file.c_str());
            return -1;
        }
        m_meshFiles[file] = meshId;
        return meshId;
    }

    MeshData data;
    if (!parseMesh(file, data))
    {
        return -1;
    }

    ResourceId meshId =
        createMesh(data.m_vertices, data.m_indices, data.m_normals, data.m_uvs, data.m_type);
    if (meshId == -1)
    {
        spdlog::error("Failed to create mesh resource id from file {}.", file.c_str());
        return -1;
    }
    m_meshFiles[file] = meshId;
    return meshId;
}

bool ResourceManager::parseMesh(const std::string& file, MeshData& data)
{
    // Retrieve file extension
    auto pos = file.find_last_of('.');
    if (pos == std::string::npos)
    {
        spdlog::error("The mesh file {} does not have a file extension and could not be loaded.",
                   file.c_str());
        return false;
    }
    std::string extension = file.substr(file.find_last_of('.') + 1);

    // Decide loading method based on extension
    // TODO Register loader functions for extensions
    if (extension == "obj")
    {
        // Wavefront OBJ file format loaded with tinyobj
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;
        // Load as obj
        // The material file is resolved relative to the mesh file
        std::string baseDir = file.substr(0, file.find_last_of('/') + 1);
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str(),
                               baseDir.c_str()))
        {
            spdlog::error("{}", err.c_str());
            return false;
        }
        if (!warn.empty())
        {
            spdlog::warn("While loading mesh file {}: {}", file.c_str(), warn);
        }
        if (shapes.size() > 1)
        {
            spdlog::error("Mesh file {} contains multiple shapes, which is not supported.",
                       file.c_str());
            return false;
        }
        if (shapes.empty())
        {
            spdlog::error("No mesh data loaded from file {}.", file.c_str());
            return false;
        }

// tinyobj keeps attributes in shared arrays indexed per corner
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> uvs;
        bool hasNormals = true;
        bool hasUvs = true;
        for (const tinyobj::index_t& index : shapes.at(0).mesh.indices)
        {
            if (index.vertex_index < 0)
            {
                spdlog::error("Mesh file {} has a face corner without a position.", file.c_str());
                return false;
            }
            vertices.insert(vertices.end(), attrib.vertices.begin() + index.vertex_index * 3,
                            attrib.vertices.begin() + index.vertex_index * 3 + 3);
            if (index.normal_index < 0)
            {
                hasNormals = false;
            }
            else
            {
                normals.insert(normals.end(), attrib.normals.begin() + index.normal_index * 3,
                               attrib.normals.begin() + index.normal_index * 3 + 3);
            }
            if (index.texcoord_index < 0)
            {
                hasUvs = false;
            }
            else
            {
                uvs.insert(uvs.end(), attrib.texcoords.begin() + index.texcoord_index * 2,
                           attrib.texcoords.begin() + index.texcoord_index * 2 + 2);
            }
        }
        if (!hasNormals)
        {
            normals.clear();
        }
        if (!hasUvs)
        {
            uvs.clear();
        }

        data = MeshData(vertices, {}, normals, uvs, PrimitiveType::Triangle);
    }
    else
    {
        spdlog::error("Unsupported mesh file format {} for file {}.", extension, file.c_str());
        return false;
    }
    return true;
}

bool ResourceManager::getMesh(ResourceId id, std::vector<float>& vertices,
                               std::vector<unsigned int>& indices, std::vector<float>& normals,
                               std::vector<float>& uvs, PrimitiveType& type) const
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

ResourceId ResourceManager::createImage(const std::vector<unsigned char>& imageData,
                                         unsigned int width, unsigned int height,
                                         ColorFormat format)
{
    // Create image
    ResourceId id = m_nextImageId;
    ++m_nextImageId;

    // TODO Sanity check if mesh id exists?
    // Add mesh
    m_images[id] = Image(imageData, width, height, format);

    // Notify listener with create event
    notifyResourceListeners(ResourceType::Image, id, ListenerEvent::Create);
    return id;
}

ResourceId ResourceManager::loadImage(const std::string& file, ColorFormat format)
{
    std::string imageKey = file + "|" + std::to_string(static_cast<int>(format));
    auto entry = m_imageFiles.find(imageKey);
    if (entry != m_imageFiles.end())
    {
        return entry->second;
    }

    // Publish a preloaded image
    auto pending = m_pendingImages.find(imageKey);
    if (pending != m_pendingImages.end())
    {
        const Image& image = pending->second;
        ResourceId imageId = createImage(image.m_data, image.m_width, image.m_height, image.m_format);
        m_pendingImages.erase(pending);
        if (imageId == -1)
        {
            spdlog::error("Failed to create image resource id from file {}.", file.c_str());
            return -1;
        }
        m_imageFiles[imageKey] = imageId;
        return imageId;
    }

    Image image;
    if (!decodeImage(file, format, image))
    {
        return -1;
    }

    ResourceId imageId = createImage(image.m_data, image.m_width, image.m_height, image.m_format);
    if (imageId == -1)
    {
        spdlog::error("Failed to create image resource id from file {}.", file.c_str());
        return -1;
    }
    m_imageFiles[imageKey] = imageId;
    return imageId;
}

bool ResourceManager::decodeImage(const std::string& file, ColorFormat format, Image& image)
{
    // TODO Check extension, png format assumed
    // PNG type check
    if (file.find(".png") == std::string::npos)
    {
        spdlog::error("Unknown file format encountered while loading image file {}.", file.c_str());
        return false;
    }

    // Map to the desired channel count
    int channels;
    switch (format)
    {
    case ColorFormat::GreyScale8:
        channels = 1;
        break;
    case ColorFormat::RGB24:
        channels = 3;
        break;
    case ColorFormat::RGBA32:
        channels = 4;
        break;
    default:
        spdlog::error("Unknown color format encountered while loading image file {}.", file.c_str());
        return false;
        break;
    }

    // Decode image data
    int decodedWidth = 0;
    int decodedHeight = 0;
    unsigned char* decoded = stbi_load(file.c_str(), &decodedWidth, &decodedHeight, nullptr, channels);
    if (decoded == nullptr)
    {
        spdlog::error("An error occured while decoding the image file {}: {}", file.c_str(),
                   stbi_failure_reason());
        return false;
    }
    std::vector<unsigned char> data(decoded, decoded + (decodedWidth * decodedHeight * channels));
    stbi_image_free(decoded);

    image = Image(data, static_cast<unsigned int>(decodedWidth),
                  static_cast<unsigned int>(decodedHeight), format);
    return true;
}

bool ResourceManager::getImage(ResourceId id, std::vector<unsigned char>& data,
                                unsigned int& width, unsigned int& height,
                                ColorFormat& format) const
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

ResourceId ResourceManager::createMaterial(ResourceId diffuse, ResourceId normal,
                                            ResourceId specular, ResourceId glow, ResourceId alpha,
                                            ResourceId customShader)
{
    // Create material
    ResourceId id = m_nextMaterialId;
    ++m_nextMaterialId;

    // Add material
    m_materials[id] = MaterialData(diffuse, normal, specular, glow, alpha, customShader);

    // Notify listener with create event
    notifyResourceListeners(ResourceType::Material, id, ListenerEvent::Create);
    return id;
}

ResourceId ResourceManager::loadMaterial(const std::string& file)
{
    auto entry = m_materialFiles.find(file);
    if (entry != m_materialFiles.end())
    {
        return entry->second;
    }

    spdlog::debug("Loading material from file {}.", file.c_str());
    IniFile ini;
    if (!ini.load(file))
    {
        spdlog::error("Failed to load material file {} as ini file.", file.c_str());
        return -1;
    }

    ResourceId textureIds[kMaterialTextureSlots];
    for (int i = 0; i < kMaterialTextureSlots; ++i)
    {
        textureIds[i] = invalidResource;
    }
    for (int i = 0; i < kMaterialTextureSlots; ++i)
    {
        const char* slot = kMaterialTextureSlotNames[i];
        if (!ini.hasKey(slot, "file"))
        {
            continue;
        }
        textureIds[i] = loadImage(ini.getValue(slot, "file", "error"), kMaterialTextureFormats[i]);
        if (textureIds[i] == -1)
        {
            spdlog::error("Failed to load {} texture specified in material file {}.", slot,
                       file.c_str());
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
            spdlog::error("Failed to load custom shader file specified in material file {}.",
                       file.c_str());
            return -1;
        }
    }

    ResourceId materialId = createMaterial(textureIds[0], textureIds[1], textureIds[2],
                                           textureIds[3], textureIds[4], customShaderId);
    if (materialId == -1)
    {
        spdlog::error("Failed to create material resource id for material file {}.", file.c_str());
        return -1;
    }
    m_materialFiles[file] = materialId;
    return materialId;
}

bool ResourceManager::getMaterialImages(const std::string& file,
                                        std::vector<std::pair<std::string, ColorFormat>>& images)
{
    IniFile ini;
    if (!ini.load(file))
    {
        spdlog::error("Failed to load material file {} as ini file.", file.c_str());
        return false;
    }

    for (int i = 0; i < kMaterialTextureSlots; ++i)
    {
        const char* slot = kMaterialTextureSlotNames[i];
        if (!ini.hasKey(slot, "file"))
        {
            continue;
        }
        images.emplace_back(ini.getValue(slot, "file", "error"), kMaterialTextureFormats[i]);
    }
    return true;
}

bool ResourceManager::getMaterial(ResourceId id, ResourceId& diffuse, ResourceId& normal,
                                   ResourceId& specular, ResourceId& glow, ResourceId& alpha,
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
    normal = iter->second.m_normal;
    specular = iter->second.m_specular;
    glow = iter->second.m_glow;
    alpha = iter->second.m_alpha;
    customShader = iter->second.m_customShader;
    return true;
}

ResourceId ResourceManager::createString(const std::string& text)
{
    // Create string
    ResourceId id = m_nextStringId;
    ++m_nextStringId;

    // Add string
    m_strings[id] = text;

    // Notify listener with create event
    notifyResourceListeners(ResourceType::String, id, ListenerEvent::Create);
    return id;
}

bool ResourceManager::getString(ResourceId id, std::string& text) const
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

ResourceId ResourceManager::createShader(ResourceId vertex, ResourceId tessCtrl,
                                          ResourceId tessEval, ResourceId geometry,
                                          ResourceId fragment)
{
    // Create shader
    ResourceId id = m_nextShaderId;
    ++m_nextShaderId;

    // Add shader
    m_shaders[id] = Shader(vertex, tessCtrl, tessEval, geometry, fragment);

    // Notify listener with create event
    notifyResourceListeners(ResourceType::Shader, id, ListenerEvent::Create);
    return id;
}

bool ResourceManager::getShader(ResourceId id, ResourceId& vertex, ResourceId& tessCtrl,
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

ResourceId ResourceManager::loadShader(const std::string& file)
{
    // Check if shader exists
    auto entry = m_shaderFiles.find(file);
    if (entry != m_shaderFiles.end())
    {
        return entry->second;
    }

	spdlog::debug("Loading shader from file {}.", file.c_str());

    // Load shader ini
    IniFile ini;
    if (!ini.load(file))
    {
        spdlog::error("Failed to load shader program file {}", file.c_str());
        return -1;
    }

    // Requires vertex and fragment shader files
    if (!ini.hasKey("vertex", "file") || !ini.hasKey("fragment", "file"))
    {
        spdlog::error(
            "The shader program file {} is missing a vertex and/or fragment shader source file "
            "specifier.",
            file.c_str());
        return -1;
    }

    ResourceId vertexId = loadString(ini.getValue("vertex", "file", "error"));
    if (vertexId == -1)
    {
        spdlog::error(
            "The vertex shader source file {}, specified in the shader program file {} could not "
            "be loaded.",
            ini.getValue("vertex", "file", "error").c_str(), file.c_str());
        return -1;
    }

	ResourceId fragmentId = loadString(ini.getValue("fragment", "file", "error"));
    if (fragmentId == -1)
    {
        spdlog::error(
            "The fragment shader source file {}, specified in the shader program file {} could not "
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
            spdlog::error(
                "The tessellation control shader source file {}, specified in the shader program "
                "file {} could not be loaded.",
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
            spdlog::error(
                "The tessellation evaluation shader source file {}, specified in the shader "
                "program file {} could not be loaded.",
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
            spdlog::error(
                "The geometry shader source file {}, specified in the shader program file {} could "
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
        spdlog::error("Failed to create reasource id for shader file {}.", file.c_str());
        return -1;
    }
    m_shaderFiles[file] = shaderId;
    return shaderId;
}

void ResourceManager::addResourceListener(IResourceListener* listener)
{
    m_resourceListeners.push_back(listener);
    listener->onAttach(this);
}

void ResourceManager::preload(const std::vector<std::string>& meshFiles,
                              const std::vector<std::string>& materialFiles)
{
    // Collect the image files of the materials, each material only once
    std::set<std::string> uniqueMaterials;
    for (const std::string& material : materialFiles)
    {
        uniqueMaterials.insert(material);
    }

    std::vector<std::pair<std::string, ColorFormat>> imageFiles;
    for (const std::string& material : uniqueMaterials)
    {
        getMaterialImages(material, imageFiles);
    }

    // Skip files that are already loaded, each file only once
    std::vector<std::string> meshes;
    std::set<std::string> uniqueMeshes;
    for (const std::string& file : meshFiles)
    {
        if (!uniqueMeshes.insert(file).second)
        {
            continue;
        }
        if (m_meshFiles.find(file) == m_meshFiles.end() && m_pendingMeshes.find(file) == m_pendingMeshes.end())
        {
            meshes.push_back(file);
        }
    }
    std::vector<std::pair<std::string, ColorFormat>> images;
    std::set<std::string> uniqueImages;
    for (const auto& image : imageFiles)
    {
        std::string key = image.first + "|" + std::to_string(static_cast<int>(image.second));
        if (!uniqueImages.insert(key).second)
        {
            continue;
        }
        if (m_imageFiles.find(key) == m_imageFiles.end() && m_pendingImages.find(key) == m_pendingImages.end())
        {
            images.push_back(image);
        }
    }

    if (meshes.empty() && images.empty())
    {
        return;
    }

    // Parse and decode the files on worker threads
    std::vector<MeshData> meshData(meshes.size());
    std::vector<char> meshOk(meshes.size(), 0);
    std::vector<Image> imageData(images.size());
    std::vector<char> imageOk(images.size(), 0);

    // One queue for both kinds of work, so the slowest job starts immediately.
    // The images are the slow part, so they go first.
    std::atomic<size_t> nextJob = 0;
    const size_t jobCount = meshes.size() + images.size();

    auto worker = [&]() {
        while (true)
        {
            size_t i = nextJob.fetch_add(1);
            if (i >= jobCount)
            {
                break;
            }
            if (i < images.size())
            {
                imageOk[i] = decodeImage(images[i].first, images[i].second, imageData[i]) ? 1 : 0;
            }
            else
            {
                size_t mesh = i - images.size();
                meshOk[mesh] = parseMesh(meshes[mesh], meshData[mesh]) ? 1 : 0;
            }
        }
    };

    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0)
    {
        threadCount = 1;
    }
    threadCount = std::min<unsigned int>(threadCount, static_cast<unsigned int>(jobCount));

    std::vector<std::thread> workers;
    for (unsigned int i = 0; i < threadCount; ++i)
    {
        workers.emplace_back(worker);
    }
    for (std::thread& w : workers)
    {
        w.join();
    }

    // Publish the preloaded resources when they are loaded for the first time
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        if (meshOk[i])
        {
            m_pendingMeshes[meshes[i]] = std::move(meshData[i]);
        }
    }
    for (size_t i = 0; i < images.size(); ++i)
    {
        if (imageOk[i])
        {
            std::string key = images[i].first + "|" + std::to_string(static_cast<int>(images[i].second));
            m_pendingImages[key] = std::move(imageData[i]);
        }
    }
}

void ResourceManager::removeResourceListener(IResourceListener* listener)
{
    m_resourceListeners.remove(listener);
    listener->onDetach(this);
}

void ResourceManager::notifyResourceListeners(ResourceType type, ResourceId id,
                                               ListenerEvent event)
{
    for (const auto& listener : m_resourceListeners)
    {
        listener->notify(type, id, event, this);
    }
}

ResourceId ResourceManager::loadString(const std::string& file)
{
	auto iter = m_textFiles.find(file);
	if (iter != m_textFiles.end())
	{
		return iter->second;
	}

	spdlog::debug("Loading text from file {}.", file.c_str());
	std::ifstream ifs(file);
	if (!ifs.is_open())
	{
		spdlog::error("Failed to open the text file {}.", file.c_str());
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
		spdlog::error("Failed to create string id for text file {}.", file.c_str());
		return -1;
	}

	m_textFiles[file] = stringId;
	return stringId;
}
