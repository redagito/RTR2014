#pragma once

#include "ARenderer.h"

/**
* \brief Deferred renderer implementation.
*/
class CDeferredRenderer : public ARenderer
{
public:
	CDeferredRenderer(const std::shared_ptr<IResourceManager>& resourceManager);
	~CDeferredRenderer();

	void draw(const IScene& scene, const ICamera& camera, const IWindow& window);

private:
};