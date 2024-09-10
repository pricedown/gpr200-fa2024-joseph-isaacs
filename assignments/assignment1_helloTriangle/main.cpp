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

// HACK: writing vertex shader source code in a string. (R syntax is to avoid using constant \ns)

// VERTEX SHADERS inputs a vertex and transforms it for clipping space
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
}
)";

// FRAGMENT SHADER outputs color of individual pixel
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
	glfwMakeContextCurrent(window); // this is what we're rendering to
	// glad literally just identifies the headers for interacting with our machine
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}



	// Copying data from CPU to GPU...j

	// GEOMETRY DATA

	unsigned int VAO; // vertex array object
	glGenVertexArrays(1, &VAO); 
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO); // generate one buffer, assigns buffer id (&VBO) a value
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW); // GL_STATIC_DRAW (for static meshes), opposed to GL_STREAM_DRAW, GL_DYNAMIC_DRAW (change almost every frame)

	// vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the buffer


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

	// SHADER PROGRAM (connect the pipeline)

	unsigned int shaderProgram = glCreateProgram(); 
	glAttachShader(shaderProgram, vertexShader); // attaches shaders to program
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Catch linker errors
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}


	// Render loop
	int frameCount = 0;
	while (!glfwWindowShouldClose(window)) {
		// INPUT
		glfwPollEvents(); // Polls inputs

		// UPDATE

		// DRAW

		// "gl-calls interface with GPU" - winebrenner

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // "Clearing" means setting all to one value
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		// draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3); // Primitive, 0, 3 vertices

		glfwSwapBuffers(window); // double-buffering
		// DEBUGS
		frameCount++;
	}
	printf("Shutting down...");
}
