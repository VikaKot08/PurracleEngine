#include "Renderable.h"
#include <cmath>

glm::mat4 Renderable::GetMatrix() const
{
    glm::mat4 localMatrix = localTransform;

    glm::mat4 trs = glm::translate(glm::mat4(1.0f), position);
    trs = trs * glm::mat4_cast(rotationQuat); // Use quaternion instead of euler
    trs = glm::scale(trs, scale);

    localMatrix = trs * localMatrix;

    if (parent)
    {
        return parent->GetMatrix() * localMatrix;
    }

    return localMatrix;
}

void Renderable::SetMatrix(const glm::mat4& matrix)
{
    glm::mat4 localMatrix = matrix;
    if (parent)
    {
        glm::mat4 parentMatrix = parent->GetMatrix();
        localMatrix = glm::inverse(parentMatrix) * matrix;
    }

    glm::vec3 skew;
    glm::vec4 perspective;

    if (glm::decompose(localMatrix, scale, rotationQuat, position, skew, perspective))
    {
        rotationQuat = glm::normalize(rotationQuat);

        glm::vec3 eulerRad = glm::eulerAngles(rotationQuat);
        rotation = glm::degrees(eulerRad);

        auto normalizeAngle = [](float angle) -> float 
            {
            while (angle > 180.0f) angle -= 360.0f;
            while (angle < -180.0f) angle += 360.0f;
            if (fabs(angle) < 0.0001f) angle = 0.0f;
            return angle;
            };

        rotation.x = normalizeAngle(rotation.x);
        rotation.y = normalizeAngle(rotation.y);
        rotation.z = normalizeAngle(rotation.z);
    }
}

void Renderable::SetRotationFromEuler(const glm::vec3& eulerDegrees)
{
    rotation = eulerDegrees;

    glm::vec3 eulerRad = glm::radians(eulerDegrees);
    rotationQuat = glm::quat(eulerRad);
    rotationQuat = glm::normalize(rotationQuat);
}

void Renderable::SetParent(Renderable* aParent)
{
	if (parent == aParent)
		return;

	if (parent)
		parent->RemoveChild(this);

	if (aParent)
		aParent->AddChild(this);
	else
		parent = nullptr;
}

void Renderable::AddChild(Renderable* aChild)
{
	if (!aChild) return;
	if (aChild == this) return;

	Renderable* ancestor = this;
	while (ancestor)
	{
		if (ancestor == aChild)
			return;
		ancestor = ancestor->parent;
	}

	glm::mat4 childWorldTransform = aChild->GetMatrix();

	if (aChild->parent)
		aChild->parent->RemoveChild(aChild);

	aChild->parent = this;
	children.push_back(aChild);

	aChild->SetMatrix(childWorldTransform);
}

void Renderable::RemoveChild(Renderable* aChild)
{
	if (!aChild) return;
	auto it = std::find(children.begin(), children.end(), aChild);
	if (it != children.end())
	{
		glm::mat4 childWorldTransform = aChild->GetMatrix();

		children.erase(it);
		aChild->parent = nullptr;

		aChild->SetMatrix(childWorldTransform);
	}
}