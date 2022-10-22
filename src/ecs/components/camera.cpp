#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

void Camera::updateCameraVectors(std::shared_ptr<Transform> transform) {
	auto yaw = transform->rotation.y;
	auto pitch = transform->rotation.x;
	front = glm::normalize(
		glm::vec3(
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		)
	);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::move(std::shared_ptr<Transform> transform, CameraDirection dir, float delta) {
	switch (dir) {
		case FORWARD:
			transform->position += front * speed * delta;
			break;
		case BACKWARD:
			transform->position -= front * speed * delta;
			break;
		case LEFT:
			transform->position -= right * speed * delta;
			break;
		case RIGHT:
			transform->position += right * speed * delta;
			break;
		case UP:
			transform->position += up * speed * delta;
			break;
		case DOWN:
			transform->position -= up * speed * delta;
			break;
	}
}

void Camera::processCursor(std::shared_ptr<Transform> transform, float xOffset, float yOffset, float delta, bool constrainPitch) {
	transform->rotation.y += xOffset * sensitivity * delta;
	transform->rotation.x += yOffset * sensitivity * delta;

	if (constrainPitch)
		transform->rotation.x = std::clamp(transform->rotation.x, -89.0f, 89.0f);

	updateCameraVectors(transform);
}