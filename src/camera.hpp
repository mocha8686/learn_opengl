#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraDirection {
	FORWARD,
	LEFT,
	BACKWARD,
	RIGHT,
	UP,
	DOWN,
};

namespace cameraDefaultVal {
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 1.0f;
}

class Camera {
	private:
		glm::vec3 position;
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;
		float yaw;
		float pitch;
		float speed = cameraDefaultVal::SPEED;
		float sensitivity = cameraDefaultVal::SENSITIVITY;

		void updateCameraVectors();

	public:
		Camera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 worldUp = glm::vec3(0.0f, 0.1f, 0.0f),
			float yaw = cameraDefaultVal::YAW,
			float pitch = cameraDefaultVal::PITCH
		) : position(position), worldUp(worldUp), yaw(yaw), pitch(pitch)
		{ updateCameraVectors(); };

		Camera(
			float posX, float posY, float posZ,
			float worldUpX, float worldUpY, float worldUpZ,
			float yaw, float pitch
		) : position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(worldUpX, worldUpY, worldUpZ)), yaw(yaw), pitch(pitch)
		{ updateCameraVectors(); };

		glm::mat4 getViewMatrix() const { return glm::lookAt(position, position + front, up); };
		glm::vec3 getPosition() const { return position; };
		void processKeyboard(CameraDirection dir, float delta);
		void processCursor(float xOffset, float yOffset, float delta, bool constrainPitch = true);
};