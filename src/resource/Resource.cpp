#include "Resource.h"

#include "core/CResourceManager.h"

IResourceManager* createResourceManager()
{
	return new CResourceManager;
}