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

// TODO: abstract into a class

float vertices[] = {
	// position			// color
	 0.5f,  0.5f, 0.0f, 0.0f, 0.5f, 0.5f, // top right
	 0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.5f, // bottom right
	-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f, 0.5f, 0.5f, 0.5f, // top left 
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

// HACK: writing vertex shader source code in a string. (R syntax is to avoid using constant \ns)

// VERTEX SHADERS transforms vectors for clipping space
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 fColor;

uniform float waveT;

void main() {
	gl_Position = vec4(cos(waveT*2+aPos.y)/1.5f * aPos.x, aPos.y + sin(waveT+(aPos.x))/2.0f, aPos.z, 1.0f);
	fColor = aColor;
}
)";

// FRAGMENT SHADER outputs pixel color
const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;
uniform float alphaShift;
uniform vec3 chromaColor;

in vec3 fColor;


void main()
{
	for (int i = 0; i < 3; i++) {
		FragColor[i] = mod(fColor[i] * chromaColor[i], 1.0f);
	}
} 
)";

int main() {
	printf("Initializing...");
	GLFWwindow* window;
	{
		// glfw.org, a library separate from opengl captures inputs and puts onto screen
		if (!glfwInit()) {
			printf("GLFW failed to init!");
			return 1;
		}
		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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
	}

	// GEOMETRY DATA

	// Vertex Array: the final vertex input
	unsigned int VAO;
	glGenVertexArrays(1, &VAO); // gen 1 buffer, initializes buffer id (&VAO)
	glBindVertexArray(VAO);

	// Vertex Buffer: raw vertex data
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // The GL_ARRAY_BUFFER target for buffer objects represents the intent to use that buffer object for vertex attribute data.

	// Element Buffer: shapes from indeces of vertices
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 

	// linking vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexArrayAttrib(VAO, 0); // Enables the 0th attribute for use in vertex shaders

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void*)(sizeof(float)*3));
	glEnableVertexArrayAttrib(VAO, 1); // Enables the 0th attribute for use in vertex shaders

	// send vertex data from CPU to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_STATIC_DRAW (for static meshes), opposed to GL_STREAM_DRAW, GL_DYNAMIC_DRAW (change almost every frame)
	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
	// send indices data 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

	// SHADERS

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // define shader type
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // supply shader source
	glCompileShader(vertexShader); // compile shader
	// GLSL is super flexible and the compilation can be done at runtime!! we can play with this like in https://shadertoy.com)
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

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); 
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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
	// attach shaders
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	// link it together
	glLinkProgram(shaderProgram);
	// catch linker errors
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
	float rad = 0;

	while (!glfwWindowShouldClose(window)) {
		// DEBUGS
		frameCount++;

		// INPUT
		glfwPollEvents(); // Polls inputs

		// UPDATE
		glUseProgram(shaderProgram);
		float time = glfwGetTime();

		// chroma color
		int chromaColorLocation = glGetUniformLocation(shaderProgram, "chromaColor");
		float r = sin(time) / 2.0f + 0.5f;
		float g = cos(time) / 2.0f + 0.5f;
		float b = -sin(time) / 2.0f + 0.5f;
		glUniform3f(chromaColorLocation, r, g, b);

		// rotation
		float waveT = time;
		int waveTLocation = glGetUniformLocation(shaderProgram, "waveT");
		glUniform1f(waveTLocation, waveT);

		// DRAW

		// "gl-calls interface with GPU" - winebrenner

		// set bg
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // "Clearing" means setting all to one value
		glClear(GL_COLOR_BUFFER_BIT);

		// draw triangle
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window); // double-buffering
	}
	printf("Shutting down...");
}
