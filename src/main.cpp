#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const unsigned int INITIAL_SCREEN_WIDTH = 800;
const unsigned int INITIAL_SCREEN_HEIGHT = 600;

const char *VERTEX_SHADER_SOURCE = "#version 330 core \
	layout (location = 0) in vec3 aPos; \
	layout (location = 1) in vec3 aCol; \
	out vec3 color; \
	void main() { \
		gl_Position = vec4(aPos, 1.0); \
		color = aCol; \
	}";
const char *FRAGMENT_SHADER_SOURCE = "#version 330 core \
	in vec3 color; \
	out vec4 FragColor; \
	void main() { \
		FragColor = vec4(color, 1.0); \
	}";

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}

// Helper functions
GLFWwindow* initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow* window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

unsigned int initializeShaderProgram() {
	// Initialize vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VERTEX_SHADER_SOURCE, NULL);
	glCompileShader(vertexShader);

	// Check compilation status of vertex shader
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
		return 0;
	}

	// Initialize fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_SOURCE, NULL);
	glCompileShader(fragmentShader);

	// Check compilation status of fragment shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
		return 0;
	}

	// Create shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check link status of shader program
	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
		return 0;
	}

	// Use program and free shaders
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

int main() {
	GLFWwindow* window = initializeGLFW();
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		return 1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		return 2;
	}

	unsigned int shaderProgram = initializeShaderProgram();

	const float VERTICES[] = {
		// position			// color
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,	0.0f, 0.0f, 1.0f,
	};

	// Initialize, bind, and configure vertex buffer object and vertex array object
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO/VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Input/render loop
	while (!glfwWindowShouldClose(window)) {
		// Input
		processInput(window);

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Deallocate resources after program lifetime
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
}
