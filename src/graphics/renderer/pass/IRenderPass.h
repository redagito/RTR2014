#pragma once

class IResourceManager;

class IRenderPass
{
   public:
    virtual ~IRenderPass();

    virtual bool init(IResourceManager* manager) = 0;
};