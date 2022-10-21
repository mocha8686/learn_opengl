#pragma once

#include "input/camera.hpp"
#include "input/keyboard.hpp"
#include "util/cache.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iosfwd>
#include <memory>
#include <string>

class Model;
class ShaderProgram;
class Texture;

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
		Cache<std::string, Texture> textures {};
		Cache<std::string, ShaderProgram> shaders {};
		Cache<std::string, Model> models {};
		bool firstMouse = true;

		Context();
		~Context();

		std::shared_ptr<ShaderProgram> compileShader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath);
		std::shared_ptr<Model> loadModel(const std::string &path);

		void moveCamera(CameraDirection dir) { camera.move(dir, time.delta); };
		void loop();
};