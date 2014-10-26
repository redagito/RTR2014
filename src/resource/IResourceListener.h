#pragma once

#include "ResourceConfig.h"

class IResourceManager;

/**
 * \brief Resource listener interface class.
 * Receives resource events after being registered to a resource manager.
 */
class IResourceListener
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IResourceListener();

    /**
     * \brief Called on attaching listener.
     */
    virtual void onAttach(IResourceManager* resourceManager) = 0;

    /**
     * \brief Called on detaching listener.
     */
    virtual void onDetach(IResourceManager* resourceManager) = 0;

    /**
     * \brief Called on resource event.
     */
    virtual void notify(EResourceType type, ResourceId id, EListenerEvent event,
                        IResourceManager* resourceManager) = 0;
};
