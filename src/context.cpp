#include "context.hpp"

#include "ecs/components/light.hpp"
#include "ecs/components/transform.hpp"
#include "ecs/scene.hpp"
#include "graphics/model.hpp"
#include "graphics/shader.hpp"
#include "input/keyboard.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

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
	if (width == screen.width && height == screen.height) return;
	if (width == 0 && height == 0) return;

	screen.width = width;
	screen.height = height;
	glViewport(0, 0, width, height);
	firstMouse = true;
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

std::shared_ptr<ShaderProgram> Context::compileShader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath) {
	auto shaderOpt = shaders.get(vertexSourcePath + ":" + fragmentSourcePath);
	if (!shaderOpt) {
		auto shader = std::make_shared<ShaderProgram>(vertexSourcePath, fragmentSourcePath);
		shaders.set(vertexSourcePath + ":" + fragmentSourcePath, shader);
		shaderOpt = shaders.get(vertexSourcePath + ":" + fragmentSourcePath);
	}
	return shaderOpt.value();
}

std::shared_ptr<Model> Context::loadModel(const std::string &path) {
	auto modelOpt = models.get(path);
	if (!modelOpt) {
		auto model = std::make_shared<Model>(*this, path);
		models.set(path, model);
		modelOpt = models.get(path);
	}
	return modelOpt.value();
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
		ctx.firstMouse = true;
	});

	auto globalShader = compileShader("res/globalVertex.glsl", "res/globalFrag.glsl");
	auto lightSourceShader = compileShader("res/lightSourceVertex.glsl", "res/lightSourceFrag.glsl");

	globalShader->uniformFloat("material.shininess", 32.0f);

	const glm::vec3 LIGHT_SOURCE_POSITIONS[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};

	Scene scene;

	auto backpack = scene.createEntity();
	auto backpackModel = loadModel("res/backpack/backpack.obj");
	Transform backpackTransform;
	backpackTransform.scale = glm::vec3(0.5f);
	scene.addComponent(backpack, backpackTransform);
	scene.addComponent(backpack, backpackModel);
	scene.addComponent(backpack, globalShader);

	auto directionalLight = scene.createEntity();
	Transform directionalLightTransform(glm::vec3(0.0f), glm::vec3(-0.2f, -1.0f, -0.3f));
	Light directionalLightComponent(DIRECTIONAL);
	directionalLightComponent.ambient = glm::vec3(0.05f);
	directionalLightComponent.diffuse = glm::vec3(0.4f);
	directionalLightComponent.specular = glm::vec3(0.5f);
	scene.addComponent(directionalLight, directionalLightTransform);
	scene.addComponent(directionalLight, directionalLightComponent);

	auto sphereModel = loadModel("res/only_quad_sphere.obj");
	for (int i = 0; i < 4; i++) {
		auto pointLight = scene.createEntity();
		Transform pointLightTransform(LIGHT_SOURCE_POSITIONS[i]);
		pointLightTransform.scale = glm::vec3(0.2f);
		Light pointLightComponent(POINT);
		scene.addComponent(pointLight, pointLightTransform);
		scene.addComponent(pointLight, sphereModel);
		scene.addComponent(pointLight, lightSourceShader);
		scene.addComponent(pointLight, pointLightComponent);
	}

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

		std::vector<std::pair<std::shared_ptr<Light>, std::shared_ptr<Transform>>> lights;
		for (auto entity : scene.getActiveEntities()) {
			auto lightOpt = scene.getComponent<Light>(entity);
			if (lightOpt) {
				auto transform = scene.getComponent<Transform>(entity).value();
				lights.push_back({lightOpt.value(), transform});
			}
		}

		auto view = camera.getViewMatrix();
		auto projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(screen.width) / static_cast<float>(screen.height),
			0.1f,
			100.0f
		);

		for (auto entity : scene.getActiveEntities()) {
			auto modelOpt = scene.getComponent<Model>(entity);
			if (modelOpt) {
				auto model = modelOpt.value();
				auto transform = scene.getComponent<Transform>(entity).value();
				auto shader = scene.getComponent<ShaderProgram>(entity).value();

				unsigned int nDirectional = 0;
				unsigned int nPoint = 0;
				unsigned int nSpot = 0;
				for (const auto &[light, lightTransform] : lights) {
					switch (light->type) {
						case DIRECTIONAL:
							light->use(*shader, *transform, nDirectional++);
							break;
						case POINT:
							light->use(*shader, *transform, nPoint++);
							break;
						case SPOT:
							light->use(*shader, *transform, nSpot++);
							break;
					}
				}
				shader->tryUniformInt("nDirectionalLights", nDirectional);
				shader->tryUniformInt("nPointLights", nPoint);
				shader->tryUniformInt("nSpotLights", nSpot);

				shader->uniformMat4("view", view);
				shader->uniformMat4("projection", projection);

				glm::mat4 modelMat(1.0f);
				modelMat = glm::translate(modelMat, transform->position);
				modelMat = glm::rotate(modelMat, transform->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, transform->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, transform->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, transform->scale);
				shader->uniformMat4("model", modelMat);

				model->draw(*shader);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}