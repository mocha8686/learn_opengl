#include "context.hpp"

#include "ecs/components/camera.hpp"
#include "ecs/components/light.hpp"
#include "ecs/components/transform.hpp"
#include "ecs/entity.hpp"
#include "ecs/scene.hpp"
#include "graphics/model.hpp"
#include "graphics/shader.hpp"
#include "input/input.hpp"
#include "input/keyboard.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
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
	window(initializeGLFW()),
	input(std::make_unique<InputManager>(*this))
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
	input->resetFirstMouse();
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
	Scene scene;

	auto mainCamera = scene.createEntity();
	auto mainCameraTransform = std::make_shared<Transform>();
	auto mainCameraComponent = std::make_shared<Camera>(mainCameraTransform);
	mainCameraComponent->setMain(true);
	mainCamera->addComponent(mainCameraTransform);
	mainCamera->addComponent(mainCameraComponent);

	input->addKeyCallback(GLFW_KEY_ESCAPE, PRESS, [](auto &ctx) { glfwSetWindowShouldClose(ctx.window, true); });
	input->addKeyCallback(GLFW_KEY_W, PRESS, [mainCameraComponent, mainCameraTransform](auto &ctx) mutable { mainCameraComponent->move(mainCameraTransform, FORWARD, ctx.time.delta); });
	input->addKeyCallback(GLFW_KEY_S, PRESS, [mainCameraComponent, mainCameraTransform](auto &ctx) mutable { mainCameraComponent->move(mainCameraTransform, BACKWARD, ctx.time.delta); });
	input->addKeyCallback(GLFW_KEY_A, PRESS, [mainCameraComponent, mainCameraTransform](auto &ctx) mutable { mainCameraComponent->move(mainCameraTransform, LEFT, ctx.time.delta); });
	input->addKeyCallback(GLFW_KEY_D, PRESS, [mainCameraComponent, mainCameraTransform](auto &ctx) mutable { mainCameraComponent->move(mainCameraTransform, RIGHT, ctx.time.delta); });
	input->addKeyCallback(GLFW_KEY_SPACE, PRESS, [mainCameraComponent, mainCameraTransform](auto &ctx) mutable { mainCameraComponent->move(mainCameraTransform, UP, ctx.time.delta); });
	input->addKeyCallback(GLFW_KEY_LEFT_SHIFT, PRESS, [mainCameraComponent, mainCameraTransform](auto &ctx) mutable { mainCameraComponent->move(mainCameraTransform, DOWN, ctx.time.delta); });
	input->addKeyCallback(GLFW_KEY_Q, RISING, [](auto &ctx) {
		switch (glfwGetInputMode(ctx.window, GLFW_CURSOR)) {
			case GLFW_CURSOR_DISABLED:
				glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case GLFW_CURSOR_NORMAL:
				glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
		}
		ctx.input->resetFirstMouse();
	});
	input->addKeyCallback(GLFW_KEY_R, RISING, [](auto &ctx) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	});
	input->addKeyCallback(GLFW_KEY_F, RISING, [](auto &ctx) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	});
	input->addCursorPosCallback([mainCameraComponent, mainCameraTransform](auto &ctx, auto xOffset, auto yOffset) mutable {
		mainCameraComponent->processCursor(mainCameraTransform, xOffset, yOffset, ctx.time.delta);
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

	auto backpack = scene.createEntity();
	auto backpackModel = loadModel("res/backpack/backpack.obj");
	Transform backpackTransform;
	backpackTransform.scale = glm::vec3(0.5f);
	backpack->addComponent(backpackTransform);
	backpack->addComponent(backpackModel);
	backpack->addComponent(globalShader);

	auto directionalLight = scene.createEntity();
	Transform directionalLightTransform(glm::vec3(0.0f), glm::vec3(-65.0f, -90.0f, 0.0f));
	Light directionalLightComponent(DIRECTIONAL);
	directionalLightComponent.ambient = glm::vec3(0.05f);
	directionalLightComponent.diffuse = glm::vec3(0.4f);
	directionalLightComponent.specular = glm::vec3(0.5f);
	directionalLight->addComponent(directionalLightTransform);
	directionalLight->addComponent(directionalLightComponent);

	auto sphereModel = loadModel("res/only_quad_sphere.obj");
	for (int i = 0; i < 4; i++) {
		auto pointLight = scene.createEntity();
		Transform pointLightTransform(LIGHT_SOURCE_POSITIONS[i]);
		pointLightTransform.scale = glm::vec3(0.2f);
		Light pointLightComponent(POINT);
		pointLight->addComponent(pointLightTransform);
		pointLight->addComponent(sphereModel);
		pointLight->addComponent(lightSourceShader);
		pointLight->addComponent(pointLightComponent);
	}

	while (!glfwWindowShouldClose(window)) {
		time.now = static_cast<float>(glfwGetTime());
		time.delta = time.now - time.last;
		time.last = time.now;

		processFramebufferSize();
		input->process();

		// Render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get lights
		std::vector<std::pair<std::shared_ptr<Light>, std::shared_ptr<Transform>>> lights;
		for (auto [_, entity] : scene.getActiveEntities()) {
			auto lightOpt = entity->getComponent<Light>();
			if (lightOpt) {
				auto transform = entity->getComponent<Transform>().value();
				lights.push_back({lightOpt.value(), transform});
			}
		}

		auto view = mainCameraComponent->getViewMatrix(mainCameraTransform);
		auto projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(screen.width) / static_cast<float>(screen.height),
			0.1f,
			100.0f
		);

		for (auto [_, entity] : scene.getActiveEntities()) {
			auto modelOpt = entity->getComponent<Model>();
			if (modelOpt) {
				auto model = modelOpt.value();
				auto transform = entity->getComponent<Transform>().value();
				auto shader = entity->getComponent<ShaderProgram>().value();

				unsigned int nDirectional = 0;
				unsigned int nPoint = 0;
				unsigned int nSpot = 0;
				for (const auto &[light, lightTransform] : lights) {
					switch (light->type) {
						case DIRECTIONAL:
							light->use(shader, lightTransform, view, nDirectional++);
							break;
						case POINT:
							light->use(shader, lightTransform, view, nPoint++);
							break;
						case SPOT:
							light->use(shader, lightTransform, view, nSpot++);
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
				modelMat = glm::rotate(modelMat, glm::radians(transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, transform->scale);
				shader->uniformMat4("model", modelMat);
				shader->tryUniformMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(modelMat))));

				model->draw(shader);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}