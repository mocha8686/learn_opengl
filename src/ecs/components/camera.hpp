#pragma once

#include "transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <type_traits>

class Transform;

enum CameraDirection {
	FORWARD,
	LEFT,
	BACKWARD,
	RIGHT,
	UP,
	DOWN,
};

class Camera {
	private:
		bool main = false;

		glm::vec3 front;
		glm::vec3 right;
		glm::vec3 up;
		glm::vec3 worldUp;
		float speed = 2.5f;
		float sensitivity = 1.0f;

		void updateCameraVectors(std::shared_ptr<Transform> transform);

	public:
		Camera(std::shared_ptr<Transform> transform, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f)) : worldUp(glm::normalize(worldUp)) {
			updateCameraVectors(transform);
		};

		Camera(std::shared_ptr<Transform> transform, float worldUpX, float worldUpY, float worldUpZ) : worldUp(glm::normalize(glm::vec3(worldUpX, worldUpY, worldUpZ))) {
			updateCameraVectors(transform);
		};

		glm::mat4 getViewMatrix(std::shared_ptr<Transform> transform) const { return glm::lookAt(transform->position, transform->position + front, up); };
		glm::vec3 getFront() const { return front; };
		void move(std::shared_ptr<Transform> transform, CameraDirection dir, float delta);
		void processCursor(std::shared_ptr<Transform> transform, float xOffset, float yOffset, float delta, bool constrainPitch = true);

		void setMain(bool main) { this->main = main; };
		bool isMain() { return main; };
};