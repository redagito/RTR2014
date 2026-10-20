#include "resource/Resource.h"

#include "resource/ResourceManager.h"

std::unique_ptr<IResourceManager> createResourceManager()
{
    return std::make_unique<ResourceManager>();
}
