#include "shader.hpp"
#include "texture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

const unsigned int INITIAL_SCREEN_WIDTH = 800;
const unsigned int INITIAL_SCREEN_HEIGHT = 600;
const float MIX_SPEED = 1.0f;
const float CAMERA_SPEED = 1.0f;
const float MOUSE_SENSITIVITY = 1.0f;

float delta = 0.0f, last = 0.0f;

float mixPercent = 0.5f;

glm::vec3 cameraPos(0.0f, 0.0f, 0.3f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float mouseLastX = INITIAL_SCREEN_WIDTH / 2, mouseLastY = INITIAL_SCREEN_HEIGHT / 2;
float pitch = 0.0f, yaw = -90.0f;

// Callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos) {
	if (firstMouse) {
		mouseLastX = xPos;
		mouseLastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - mouseLastX;
	float yOffset = mouseLastY - yPos;
	mouseLastX = xPos;
	mouseLastY = yPos;

	xOffset *= MOUSE_SENSITIVITY * delta;
	yOffset *= MOUSE_SENSITIVITY * delta;

	yaw += xOffset;
	pitch = std::clamp(pitch + yOffset, -89.0f, 89.0f);

	glm::vec3 direction(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))
	);
	cameraFront = glm::normalize(direction);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		mixPercent = std::min(mixPercent + (MIX_SPEED * delta), 1.0f);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		mixPercent = std::max(mixPercent - (MIX_SPEED * delta), 0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraFront * CAMERA_SPEED * delta;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraFront * CAMERA_SPEED * delta;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED * delta;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED * delta;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraUp * CAMERA_SPEED * delta;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos -= cameraUp * CAMERA_SPEED * delta;
}

// Helper functions
GLFWwindow *initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow *window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Learn OpenGL", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	return window;
}

int main() {
	GLFWwindow *window = initializeGLFW();
	if (window == nullptr) {
		std::cerr << "Failed to create GLFW window." << std::endl;
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	ShaderProgram program("res/basicVertex.glsl", "res/basicFragment.glsl");

	const GLfloat VERTICES[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
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

	// Initialize, bind, and configure vertex buffer object and vertex array object
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO/VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	GLint stride = 5 * sizeof(GLfloat);
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 0);
	glEnableVertexAttribArray(0);
	// TexCoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Textures
	Texture
	 	tex0("res/container.jpg", GL_RGB),
		tex1("res/awesomeface.png", GL_RGBA);

	program.uniformInt("tex0", 0);
	program.uniformInt("tex1", 1);

	// Input/render loop
	while (!glfwWindowShouldClose(window)) {
		float now = glfwGetTime();
		delta = now - last;
		last = now;

		// Input
		processInput(window);

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Textures
		tex0.use(GL_TEXTURE0);
		tex1.use(GL_TEXTURE1);
		program.uniformFloat("mixPercent", mixPercent);

		glBindVertexArray(VAO);

		// Matrices
		glm::mat4 view = glm::lookAt(
			cameraPos,
			cameraPos + cameraFront,
			cameraUp
		);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) INITIAL_SCREEN_WIDTH / (float) INITIAL_SCREEN_HEIGHT, 0.1f, 100.0f);

		program.uniformMat4("view", view);
		program.uniformMat4("projection", projection);

		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, CUBE_POSITIONS[i]);
			model = glm::rotate(model, glm::radians(20.0f * i), glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f)));

			program.uniformMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Deallocate resources after program lifetime
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}
