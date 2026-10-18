#include "resource/Resource.h"

#include "resource/core/CResourceManager.h"

IResourceManager* createResourceManager()
{
	return new CResourceManager;
}
