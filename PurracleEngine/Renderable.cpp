#include "Renderable.h"

glm::mat4 Renderable::GetMatrix() const
{
	// Start with local transform (this is the object's transform relative to its parent)
	glm::mat4 localMatrix = localTransform;

	// Apply TRS transformations to the local transform
	glm::mat4 trs = glm::translate(glm::mat4(1.0f), position);
	trs = glm::rotate(trs, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	trs = glm::rotate(trs, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	trs = glm::rotate(trs, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	trs = glm::scale(trs, scale);

	// Combine: first apply local transform, then TRS
	localMatrix = trs * localMatrix;

	// If we have a parent, transform into parent's space
	if (parent)
	{
		return parent->GetMatrix() * localMatrix;
	}

	return localMatrix;
}

void Renderable::SetMatrix(const glm::mat4& matrix)
{
	// If we have a parent, we need to convert world matrix to local matrix
	glm::mat4 localMatrix = matrix;
	if (parent)
	{
		glm::mat4 parentMatrix = parent->GetMatrix();
		localMatrix = glm::inverse(parentMatrix) * matrix;
	}

	glm::vec3 skew;
	glm::vec4 perspective;
	glm::quat rotationQuat;

	// Decompose the local matrix
	if (glm::decompose(localMatrix, scale, rotationQuat, position, skew, perspective))
	{
		// Normalize the quaternion to avoid accumulation errors
		rotationQuat = glm::normalize(rotationQuat);

		// Convert quaternion to euler angles (in radians)
		glm::vec3 eulerRad = glm::eulerAngles(rotationQuat);

		// Convert to degrees and ensure they're in a reasonable range
		rotation.x = glm::degrees(eulerRad.x);
		rotation.y = glm::degrees(eulerRad.y);
		rotation.z = glm::degrees(eulerRad.z);

		// Normalize angles to [-180, 180] range to avoid large values
		auto normalizeAngle = [](float angle) -> float {
			while (angle > 180.0f) angle -= 360.0f;
			while (angle < -180.0f) angle += 360.0f;
			return angle;
			};

		rotation.x = normalizeAngle(rotation.x);
		rotation.y = normalizeAngle(rotation.y);
		rotation.z = normalizeAngle(rotation.z);
	}
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

	// Check for circular dependency
	Renderable* ancestor = this;
	while (ancestor)
	{
		if (ancestor == aChild)
			return;
		ancestor = ancestor->parent;
	}

	// Store the child's world transform before reparenting
	glm::mat4 childWorldTransform = aChild->GetMatrix();

	if (aChild->parent)
		aChild->parent->RemoveChild(aChild);

	aChild->parent = this;
	children.push_back(aChild);

	// Convert world transform to local space relative to new parent
	aChild->SetMatrix(childWorldTransform);
}

void Renderable::RemoveChild(Renderable* aChild)
{
	if (!aChild) return;
	auto it = std::find(children.begin(), children.end(), aChild);
	if (it != children.end())
	{
		// Store world transform before unparenting
		glm::mat4 childWorldTransform = aChild->GetMatrix();

		children.erase(it);
		aChild->parent = nullptr;

		// Restore world transform in world space (no parent now)
		aChild->SetMatrix(childWorldTransform);
	}
}