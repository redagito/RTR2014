#include "resource/Resource.h"

#include "resource/ResourceManager.h"

IResourceManager* createResourceManager()
{
	return new ResourceManager;
}
