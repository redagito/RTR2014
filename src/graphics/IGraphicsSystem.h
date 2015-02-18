#pragma once

class IScene;

class IGraphicsSystem
{
   public:
    virtual ~IGraphicsSystem();

    /**
    * \brief Create scene.
    */
    virtual IScene* createScene() = 0;
};