#include "context.hpp"
#include "keyboard.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <glad/glad.h> // IWYU pragma: keep
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
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

	ShaderProgram globalProgram("res/globalVertex.glsl", "res/globalFrag.glsl");
	ShaderProgram lightSourceProgram("res/lightSourceVertex.glsl", "res/lightSourceFrag.glsl");

	const GLfloat VERTICES[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	};

	const glm::vec3 CUBE_POSITIONS[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	const glm::vec3 LIGHT_SOURCE_POSITIONS[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	GLint stride = 8 * sizeof(GLfloat);
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 0);
	glEnableVertexAttribArray(0);
	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	Texture diffuseMap("res/container2.png");
	Texture specularMap("res/container2_specular.png");
	diffuseMap.use(GL_TEXTURE0);
	specularMap.use(GL_TEXTURE1);

	globalProgram.uniformInt("material.diffuseMap", 0);
	globalProgram.uniformInt("material.specularMap", 1);
	globalProgram.uniformFloat("material.shininess", 32.0f);

	globalProgram.uniformVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
	globalProgram.uniformVec3("directionalLight.properties.ambient", 0.05f);
	globalProgram.uniformVec3("directionalLight.properties.diffuse", 0.4f);
	globalProgram.uniformVec3("directionalLight.properties.specular", 0.5f);

	globalProgram.uniformVec3("spotLight.position", 0.0f);
	globalProgram.uniformVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
	globalProgram.uniformFloat("spotLight.phi", cos(glm::radians(12.5f)));
	globalProgram.uniformFloat("spotLight.gamma", cos(glm::radians(15.0f)));
	globalProgram.uniformFloat("spotLight.attenuation.linear", .09f);
	globalProgram.uniformFloat("spotLight.attenuation.quadratic", .032f);
	globalProgram.uniformVec3("spotLight.properties.ambient", 0.0f);
	globalProgram.uniformVec3("spotLight.properties.diffuse", 1.0f);
	globalProgram.uniformVec3("spotLight.properties.specular", 1.0f);

	for (int i = 0; i < 4; i++) {
		std::ostringstream lightStringStream;
		lightStringStream << "pointLights[" << i << "].";
		auto lightString = lightStringStream.str();

		globalProgram.uniformFloat(lightString + "attenuation.linear", .09f);
		globalProgram.uniformFloat(lightString + "attenuation.quadratic", .032f);

		auto propertiesString = lightStringStream.str() + "properties.";
		globalProgram.uniformVec3(propertiesString + "ambient", 0.05f);
		globalProgram.uniformVec3(propertiesString + "diffuse", 0.8f);
		globalProgram.uniformVec3(propertiesString + "specular", 1.0f);
	}

	while (!glfwWindowShouldClose(window)) {
		time.now = static_cast<float>(glfwGetTime());
		time.delta = time.now - time.last;
		time.last = time.now;

		processFramebufferSize();
		processCursorPos();
		keyboard.process();

		// Render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Matrices
		auto view = camera.getViewMatrix();
		auto projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(screen.width) / static_cast<float>(screen.height),
			0.1f,
			100.0f
		);

		glm::vec3 lightPos(0.6f + 2 * cos(time.now), 0.5f + 2.0f * sin(time.now), 2.0f);

		{ // Object
			globalProgram.uniformMat4("view", view);
			globalProgram.uniformMat4("projection", projection);

			glBindVertexArray(VAO);
			globalProgram.use();

			for (int i = 0; i < 10; i++) {
				glm::mat4 model(1.0f);
				model = glm::translate(model, CUBE_POSITIONS[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

				glm::mat3 normalMatrix(glm::transpose(glm::inverse(view * model)));

				globalProgram.uniformMat4("model", model);
				globalProgram.uniformMat3("normalMatrix", normalMatrix);

				for (int i = 0; i < 4; i++) {
					std::ostringstream lightStringStream;
					lightStringStream << "pointLights[" << i << "].";
					auto lightString = lightStringStream.str();

					auto position = LIGHT_SOURCE_POSITIONS[i];
					auto viewSpaceLightPosition = view * glm::vec4(position.x, position.y, position.z, 1.0f);

					globalProgram.uniformVec3(lightString + "position", glm::vec3(viewSpaceLightPosition.x, viewSpaceLightPosition.y, viewSpaceLightPosition.z));
				}

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		{ // Light source
			lightSourceProgram.uniformMat4("view", view);
			lightSourceProgram.uniformMat4("projection", projection);

			for (int i = 0; i < 4; i++) {
				glm::mat4 model(1.0f);
				model = glm::translate(model, LIGHT_SOURCE_POSITIONS[i]);
				model = glm::scale(model, glm::vec3(0.2f));

				lightSourceProgram.uniformMat4("model", model);

				glBindVertexArray(lightVAO);
				lightSourceProgram.use();
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
}