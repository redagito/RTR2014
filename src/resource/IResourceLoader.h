#pragma once

#include <memory>
#include <string>

class IResourceManager;

class IResourceLoader
{
   public:
    virtual bool loadMesh(std::string filePath, IResourceManager&) = 0;
};