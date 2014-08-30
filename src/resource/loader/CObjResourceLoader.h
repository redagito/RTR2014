#pragma once

#include "resource/IResourceLoader.h"

class CObjResourceLoader : public IResourceLoader
{
   public:
    virtual bool loadMesh(std::string filePath, IResourceManager&) override;
};