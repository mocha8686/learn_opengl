#pragma once

#include "util/cache.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iosfwd>
#include <memory>
#include <string>

class InputManager;
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

		GLFWwindow *window;
		std::unique_ptr<InputManager> input;
		Cache<std::string, Texture> textures {};
		Cache<std::string, ShaderProgram> shaders {};
		Cache<std::string, Model> models {};

		Context();
		~Context();

		std::shared_ptr<ShaderProgram> compileShader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath);
		std::shared_ptr<Model> loadModel(const std::string &path);

		void loop();
};