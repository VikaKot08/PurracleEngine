#pragma once
#include <vector>
#include "Renderable.h"

class Scene
{
public:
	Scene();
	void AddRenderable(Renderable* aRenderable) { objects.push_back(aRenderable); }
	std::vector<Renderable*> objects;
};

