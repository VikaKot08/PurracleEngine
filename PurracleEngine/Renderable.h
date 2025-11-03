#pragma once
#include "Shader.h"

class Renderable
{
public:
    virtual ~Renderable() = default;

    virtual void Render(Shader* myShader) = 0;
};

