#pragma once
#include "Scene.h"

class RenderPass
{
public:
	virtual void Execute(Scene& aScene) = 0;
};

