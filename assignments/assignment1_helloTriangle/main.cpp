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

#include <jisaacs/shader.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

// TODO: abstract into a class

float vertices[] = {
	// position				// color
	 0.5f,  0.5f, 0.0f,		0.0f, 0.5f, 0.5f, // top right
	 0.5f, -0.5f, 0.0f,		0.5f, 0.0f, 0.5f, // bottom right
	-0.5f, -0.5f, 0.0f,		0.5f, 0.5f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f,		0.5f, 0.5f, 0.5f, // top left 
};
unsigned int cubeIndices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

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

	// position vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexArrayAttrib(VAO, 0);
	// color vertex attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void*)(sizeof(float)*3));
	glEnableVertexArrayAttrib(VAO, 1);

	// send vertex data from CPU to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_STATIC_DRAW 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// send indices data 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW); 

	jisaacs::Shader myShader = jisaacs::Shader("assets/vertexShader.vert", "assets/fragmentShader.frag");

	// Render loop
	int frameCount = 0;
	while (!glfwWindowShouldClose(window)) {
		// DEBUGS
		frameCount++;

		// INPUT

		glfwPollEvents(); // Polls inputs

		// UPDATE

		float time = glfwGetTime();
		myShader.use();

		// chroma color
		float r = sin(time) / 2.0f + 0.5f;
		float g = cos(time) / 2.0f + 0.5f;
		float b = -sin(time) / 2.0f + 0.5f;
		myShader.setVec3("chromaColor", r, g, b);

		// wave
		myShader.setFloat("waveT", time);

		// DRAW

		// background
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // "Clearing" means setting all to one value
		glClear(GL_COLOR_BUFFER_BIT);

		// triangles
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window); // double-buffering technique
	}
	printf("Shutting down...");
}
