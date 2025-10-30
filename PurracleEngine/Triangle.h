#pragma once
#include "Renderable.h"
class Triangle : public Renderable
{
public:
    Triangle();
    ~Triangle() override;

    void Render() override;

private:
    unsigned int VBO;
    unsigned int VAO;
};

