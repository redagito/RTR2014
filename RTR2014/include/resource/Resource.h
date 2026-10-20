#pragma once

#include <memory>

#include "IResourceListener.h"
#include "IResourceManager.h"

/**
 * \brief Creates a resource manager.
 */
std::unique_ptr<IResourceManager> createResourceManager();
