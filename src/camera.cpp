#include "camera.hpp"

// IWYU pragma: begin_exports
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// IWYU pragma: end_exports

#include <algorithm> // IWYU pragma: export
#include <cmath>

void Camera::updateCameraVectors() {
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

void Camera::processKeyboard(CameraDirection dir, float delta) {
	switch (dir) {
		case FORWARD:
			position += front * speed * delta;
			break;
		case BACKWARD:
			position -= front * speed * delta;
			break;
		case LEFT:
			position -= right * speed * delta;
			break;
		case RIGHT:
			position += right * speed * delta;
			break;
		case UP:
			position += up * speed * delta;
			break;
		case DOWN:
			position -= up * speed * delta;
			break;
	}
}

void Camera::processCursor(float xOffset, float yOffset, float delta, bool constrainPitch) {
	yaw += xOffset * sensitivity * delta;
	pitch += yOffset * sensitivity * delta;

	if (constrainPitch)
		pitch = std::clamp(pitch, -89.0f, 89.0f);

	updateCameraVectors();
}