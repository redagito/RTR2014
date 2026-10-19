#include <spdlog/spdlog.h>
#include "io/CObjModelLoader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <glm/glm.hpp>


// Parse obj file
CObjModelLoader::CObjModelLoader() {}

bool CObjModelLoader::load(const std::string& file)
{
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        spdlog::error("The mesh file {} could not be opened.", file.c_str());
        return false;
    }

    // Temp data
    d_vertices.clear();
    d_normals.clear();
    d_uv.clear();

    std::vector<float> tempVert;
    std::vector<float> tempUV;
    std::vector<float> tempNorm;

    // Parse lines
    std::string line;
    while (std::getline(ifs, line))
    {
        if (!parseLine(line, tempVert, tempNorm, tempUV))
        {
            spdlog::error("Failed to parse line '{}' in mesh file {}.", line, file.c_str());
            return false;
        }
    }
    spdlog::info("Loaded obj model {} with {} triangles.", file.c_str(), d_vertices.size() / 3);
    return true;
}

// Access model data
const std::vector<float>& CObjModelLoader::getVertices() const { return d_vertices; }

const std::vector<float>& CObjModelLoader::getNormals() const { return d_normals; }

const std::vector<float>& CObjModelLoader::getUV() const { return d_uv; }

// Parsing logic
bool CObjModelLoader::parseLine(const std::string& line, std::vector<float>& tempVert,
                                std::vector<float>& tempNorm, std::vector<float>& tempUV)
{
    if (line.empty() || line[0] == '#')
    {
        // Skip empty lines and comments
        return true;
    }
    if (line[0] == 'v')
    {
        if (line.size() > 1 && line[1] == ' ')
        {
            parseVertex(line.substr(2), tempVert);
            return true;
        }
        if (line.size() > 2 && line[1] == 't')
        {
            parseUV(line.substr(3), tempUV);
            return true;
        }
        if (line.size() > 2 && line[1] == 'n')
        {
            parseNormal(line.substr(3), tempNorm);
            return true;
        }
        return false;
    }
    if (line[0] == 'f')
    {
        if (line.size() < 2)
        {
            return false;
        }
        return parseIndex(line.substr(2), tempVert, tempNorm, tempUV);
    }
    return true;
}

void CObjModelLoader::parseVertex(const std::string& line, std::vector<float>& vert)
{
    float x, y, z;
    std::stringstream ss;
    ss << line;
    ss >> x;
    ss >> y;
    ss >> z;
    vert.push_back(x);
    vert.push_back(y);
    vert.push_back(z);
    return;
}

void CObjModelLoader::parseNormal(const std::string& line, std::vector<float>& norm)
{
    float x, y, z;
    std::stringstream ss;
    ss << line;
    ss >> x;
    ss >> y;
    ss >> z;
    norm.push_back(x);
    norm.push_back(y);
    norm.push_back(z);
    return;
}

void CObjModelLoader::parseUV(const std::string& line, std::vector<float>& uv)
{
    float u, v;
    std::stringstream ss;
    ss << line;
    ss >> u;
    ss >> v;
    uv.push_back(u);
    uv.push_back(v);
    return;
}

bool CObjModelLoader::parseIndex(const std::string& line, const std::vector<float>& vert,
                                 const std::vector<float>& norm, const std::vector<float>& uv)
{
    int v = 0;
    int t = 0;
    int n = 0;
    char c;
    std::stringstream ss;
    ss << line;

    auto inRange = [](int index, size_t stride, size_t count) {
        return index >= 1 && static_cast<size_t>(index - 1) * stride + stride <= count;
    };

    for (int i = 0; i < 3; ++i)
    {
        if (!(ss >> v >> c >> t >> c >> n))
        {
            return false;
        }
        if (!inRange(v, 3, vert.size()) || !inRange(t, 2, uv.size()) ||
            !inRange(n, 3, norm.size()))
        {
            return false;
        }

        // Create data
        d_vertices.push_back(vert[(v - 1) * 3]);
        d_vertices.push_back(vert[(v - 1) * 3 + 1]);
        d_vertices.push_back(vert[(v - 1) * 3 + 2]);
        d_uv.push_back(uv[(t - 1) * 2]);
        d_uv.push_back(uv[(t - 1) * 2 + 1]);
        d_normals.push_back(norm[(n - 1) * 3]);
        d_normals.push_back(norm[(n - 1) * 3 + 1]);
        d_normals.push_back(norm[(n - 1) * 3 + 2]);
    }
    return true;
}
