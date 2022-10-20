#include "context.hpp"

#include "graphics/model.hpp"
#include "graphics/shader.hpp"
#include "input/keyboard.hpp"

#include <glad/glad.h> // IWYU pragma: keep
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

GLFWwindow *initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow *window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Learn OpenGL", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}

Context::Context() :
	screen { .width = INITIAL_WINDOW_WIDTH, .height = INITIAL_WINDOW_HEIGHT },
	time { .now = 0.0f, .delta = 0.0f, .last = 0.0f },
	cursor { .lastX = INITIAL_WINDOW_WIDTH / 2, .lastY = INITIAL_WINDOW_HEIGHT / 2 },
	camera(0.0f, 0.0f, 3.0f),
	window(initializeGLFW()),
	keyboard(*this)
{
	if (window == nullptr) {
		throw std::runtime_error("Failed to initialize GLFW.");
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD.");
	}

	glEnable(GL_DEPTH_TEST);
}

Context::~Context() {
	glfwTerminate();
}

void Context::processFramebufferSize() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	if (width == 0 && height == 0) return;

	screen.width = width;
	screen.height = height;
	glViewport(0, 0, width, height);
}

void Context::processCursorPos() {
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	if (xPos == 0 && yPos == 0) return;

	if (firstMouse) {
		cursor.lastX = xPos;
		cursor.lastY = yPos;
		firstMouse = false;
	}

	auto xOffset = xPos - cursor.lastX;
	auto yOffset = cursor.lastY - yPos;
	cursor.lastX = xPos;
	cursor.lastY = yPos;

	camera.processCursor(xOffset, yOffset, time.delta);
}

void Context::loop() {
	keyboard.addCallback(GLFW_KEY_ESCAPE, PRESS, [](auto &ctx) { glfwSetWindowShouldClose(ctx.window, true); });
	keyboard.addCallback(GLFW_KEY_W, PRESS, [](auto &ctx) { ctx.moveCamera(FORWARD); });
	keyboard.addCallback(GLFW_KEY_S, PRESS, [](auto &ctx) { ctx.moveCamera(BACKWARD); });
	keyboard.addCallback(GLFW_KEY_A, PRESS, [](auto &ctx) { ctx.moveCamera(LEFT); });
	keyboard.addCallback(GLFW_KEY_D, PRESS, [](auto &ctx) { ctx.moveCamera(RIGHT); });
	keyboard.addCallback(GLFW_KEY_SPACE, PRESS, [](auto &ctx) { ctx.moveCamera(UP); });
	keyboard.addCallback(GLFW_KEY_LEFT_SHIFT, PRESS, [](auto &ctx) { ctx.moveCamera(DOWN); });
	keyboard.addCallback(GLFW_KEY_Q, RISING, [](auto &ctx) {
		switch (glfwGetInputMode(ctx.window, GLFW_CURSOR)) {
			case GLFW_CURSOR_DISABLED:
				glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case GLFW_CURSOR_NORMAL:
				glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
		}
	});

	ShaderProgram shader("res/globalVertex.glsl", "res/globalFrag.glsl");
	ShaderProgram lightSourceShader("res/lightSourceVertex.glsl", "res/lightSourceFrag.glsl");
	Model backpack(*this, "res/backpack/backpack.obj");
	Model sphere(*this, "res/cube.obj");

	shader.uniformFloat("material.shininess", 32.0f);

	shader.uniformVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
	shader.uniformVec3("directionalLight.properties.ambient", 0.05f);
	shader.uniformVec3("directionalLight.properties.diffuse", 0.4f);
	shader.uniformVec3("directionalLight.properties.specular", 0.5f);

	shader.uniformVec3("spotLight.position", 0.0f);
	shader.uniformVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
	shader.uniformFloat("spotLight.phi", cos(glm::radians(12.5f)));
	shader.uniformFloat("spotLight.gamma", cos(glm::radians(15.0f)));
	shader.uniformFloat("spotLight.attenuation.linear", .09f);
	shader.uniformFloat("spotLight.attenuation.quadratic", .032f);
	shader.uniformVec3("spotLight.properties.ambient", 0.0f);
	shader.uniformVec3("spotLight.properties.diffuse", 1.0f);
	shader.uniformVec3("spotLight.properties.specular", 1.0f);

	for (int i = 0; i < 4; i++) {
		std::ostringstream lightStringStream;
		lightStringStream << "pointLights[" << i << "].";
		auto lightString = lightStringStream.str();

		shader.uniformFloat(lightString + "attenuation.linear", .09f);
		shader.uniformFloat(lightString + "attenuation.quadratic", .032f);

		auto propertiesString = lightStringStream.str() + "properties.";
		shader.uniformVec3(propertiesString + "ambient", 0.05f);
		shader.uniformVec3(propertiesString + "diffuse", 0.8f);
		shader.uniformVec3(propertiesString + "specular", 1.0f);
	}

	const glm::vec3 LIGHT_SOURCE_POSITIONS[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};

	while (!glfwWindowShouldClose(window)) {
		time.now = static_cast<float>(glfwGetTime());
		time.delta = time.now - time.last;
		time.last = time.now;

		processFramebufferSize();
		processCursorPos();
		keyboard.process();

		// Render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Matrices
		auto view = camera.getViewMatrix();
		auto projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(screen.width) / static_cast<float>(screen.height),
			0.1f,
			100.0f
		);

		shader.uniformMat4("view", view);
		shader.uniformMat4("projection", projection);
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, glm::vec3(0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			shader.uniformMat4("model", model);
			backpack.draw(shader);
		}

		lightSourceShader.uniformMat4("view", view);
		lightSourceShader.uniformMat4("projection", projection);
		{
			for (int i = 0; i < 4; i++) {
				glm::mat4 model(1.0f);
				model = glm::translate(model, LIGHT_SOURCE_POSITIONS[i]);
				model = glm::scale(model, glm::vec3(0.001f));
				lightSourceShader.uniformMat4("model", model);
				sphere.draw(lightSourceShader);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}