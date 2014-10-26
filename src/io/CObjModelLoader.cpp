#include "CObjModelLoader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <glm/glm.hpp>

#include "debug/Log.h"

// Parse obj file
CObjModelLoader::CObjModelLoader() {}

bool CObjModelLoader::load(const std::string& file)
{
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        LOG_ERROR("The mesh file %s could not be opened.", file.c_str());
        return false;
    }

    // Temp data
    std::vector<float> tempVert;
    std::vector<float> tempUV;
    std::vector<float> tempNorm;

    // Parse lines
    std::string line;
    while (std::getline(ifs, line))
    {
        parseLine(line, tempVert, tempNorm, tempUV);
    }
    LOG_INFO("Loaded obj model %s with %u triangles.", file.c_str(), d_vertices.size() / 3);
    return true;
}

// Access model data
const std::vector<float>& CObjModelLoader::getVertices() const { return d_vertices; }

const std::vector<float>& CObjModelLoader::getNormals() const { return d_normals; }

const std::vector<float>& CObjModelLoader::getUV() const { return d_uv; }

// Parsing logic
void CObjModelLoader::parseLine(const std::string& line, std::vector<float>& tempVert,
                                std::vector<float>& tempNorm, std::vector<float>& tempUV)
{
    if (line[0] == '#')
    {
        // Ignore comments
    }
    else if (line[0] == 'v')
    {
        if (line[1] == ' ')
        {
            // Parse as vertex
            parseVertex(line.substr(2), tempVert);
        }
        else if (line[1] == 't')
        {
            // Parse as texture coord
            parseUV(line.substr(3), tempUV);
        }
        else if (line[1] == 'n')
        {
            // Parse as normal
            parseNormal(line.substr(3), tempNorm);
        }
    }
    else if (line[0] == 'f')
    {
        // Parse as indices
        parseIndex(line.substr(2), tempVert, tempNorm, tempUV);
    }
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

void CObjModelLoader::parseIndex(const std::string& line, const std::vector<float>& vert,
                                 const std::vector<float>& norm, const std::vector<float>& uv)
{
    int v, t, n;
    char c;
    std::stringstream ss;
    ss << line;
    for (int i = 0; i < 3; ++i)
    {
        ss >> v;
        ss >> c;
        ss >> t;
        ss >> c;
        ss >> n;

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
    return;
}
