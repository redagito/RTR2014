#include "CDeferredRenderer.h"

CDeferredRenderer::CDeferredRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
	:
	ARenderer(resourceManager)
{
	return;
}

CDeferredRenderer::~CDeferredRenderer()
{
	return;
}

void CDeferredRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{

}