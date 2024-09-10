/*
We're not going to encapsulate too much starting out, it'll let us see the internal opengl calls before we abstract it away.
That means we'll have a lot of boilerplate and stuff in main, and a lot of explanatory comments
*/

#include <stdio.h>
#include <iostream>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

/*
glm::vec3 chroma(float t, float colorOffset) {
	glm::vec3 result;
	float scaledT = 2 * ew::PI * t;
	// TODO: some funny stuff
	for (int i = 0; i < 3; i++) {
		result[i] = sin(i*colorOffset + scaledT);
	}
}
*/

// TODO: abstract into a class
float cubeVertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

// WTF, writing vertex shader source code in a string. (R syntax is to avoid using constant \ns)
// vertex shader expects one input, that being the position of a vertex. It changes its position in clipping space. 
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
}
)";

// fragment shader expects one output, that being the color of the pixel
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 
)";

int main() {
	printf("Initializing...");
	// glfw.org, a library separate from opengl captures inputs and puts onto screen
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	// this is what we're rendering to
	glfwMakeContextCurrent(window);
	// glad literally just identifies the headers for interacting with our machine
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	// Init
	// ...


	// Copying data from CPU to GPU...

	// GEOMETRY DATA

	unsigned int VAO; // vertex array 
	glGenVertexArrays(1, &VAO); 
	glBindVertexArray(VAO);

	unsigned int VBO; // buffer id
	glGenBuffers(1, &VBO); // generate one buffer, assigns buffer id (&VBO) a value
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW); // GL_STATIC_DRAW (for static meshes), opposed to GL_STREAM_DRAW, GL_DYNAMIC_DRAW (change almost every frame)

	// vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// VERTEX SHADER 
	// Create shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // GL_FRAGMENT_SHADER, COMPUTE_SHADER, this is where you define shader type
	// Supply the shader source
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader); // you can do this at runtime!!!!!!!!!!!!, we can play with this like in https://shadertoy.com
	// the GLSL is super flexible
	// Also, this is the "compiling shader" section in your games
	 
	// hey, shaders can quietly fail and not really return anything on their own, so you have to do this.
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}


	// FRAGMENT SHADER 
	// Create shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); 
	// Supply the shader source
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Debug
	glCompileShader(fragmentShader);
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}


	// SHADER PROGRAM (the whole pipeline)

	// beware of linker errors, because this is where we link stuff together
	// you also need to log these yourself.

	// Create shader program object by linking shaders together
	unsigned int shaderProgram = glCreateProgram(); 
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Catch errors
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	// ASIDE: the shader program runs on the GPU. The goal is just that we're writing code to run on the GPU

	// unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Render loop
	int frameCount = 0;
	while (!glfwWindowShouldClose(window)) {
		// INPUT
		glfwPollEvents(); // Gets inputs

		// UPDATE

		// DRAW
		// Clear framebuffer
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // gl calls interface with GPU
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window); // double-buffering
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3); // Primitive, 0, 3 vertices

		// DEBUGS
		frameCount++;
	}
	printf("Shutting down...");
}
