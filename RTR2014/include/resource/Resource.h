#pragma once

#include <memory>

#include "IResourceManager.h"
#include "IResourceListener.h"

/**
 * \brief Creates a resource manager.
 */
std::unique_ptr<IResourceManager> createResourceManager();
