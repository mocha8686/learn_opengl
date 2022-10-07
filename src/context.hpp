#pragma once

#include "camera.hpp"
#include "keyboard.hpp"
#include <glad/glad.h> // IWYU pragma: keep
#include <GLFW/glfw3.h>

const unsigned int INITIAL_WINDOW_WIDTH = 800;
const unsigned int INITIAL_WINDOW_HEIGHT = 600;

class Context {
	private:
		void processFramebufferSize();
		void processCursorPos();

	public:
		struct {
			unsigned int width;
			unsigned int height;
		} screen;

		struct {
			float now;
			float delta;
			float last;
		} time;

		struct {
			float lastX;
			float lastY;
		} cursor;

		GLFWwindow *window;
		Camera camera;
		Keyboard keyboard;
		bool firstMouse = true;

		Context();
		~Context();

		void moveCamera(CameraDirection dir) { camera.move(dir, time.delta); };
		void loop();
};