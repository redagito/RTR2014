#pragma once

#include "graphics/IGraphicsSystem.h"

class IScene;

class CGraphicsSystem : public IGraphicsSystem
{
public:
	IScene* createScene();
};