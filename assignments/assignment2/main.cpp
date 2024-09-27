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
#include <jisaacs/texture.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

/*
const float vertices[] = {
	// position			 // color				  // texture coords
	-0.5f, -0.5f, 0.0f,	 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // 
	 0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // 
	 0.5f,  0.5f, 0.0f,	 1.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, // 
	-0.5f,  0.5f, 0.0f,	 0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f, // 
};
*/

const float bgVertices[] = {
	// position			 // color				  // texture coords
	-1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // 
	 1.0f, -1.0f, 0.0f,	 1.0f, 0.0f, 0.0f, 1.0f,  3.0f, 0.0f, // 
	 1.0f,  1.0f, 0.0f,	 1.0f, 1.0f, 0.0f, 1.0f,  3.0f, 3.0f, // 
	-1.0f,  1.0f, 0.0f,	 0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 3.0f, // 
};

const int indices[] = {
	0, 1, 2,
	2, 3, 0
};

int main() {
	#pragma region Initialization
	printf("Initializing...");
	GLFWwindow* window;
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
	#pragma endregion
	#pragma region Geometry data
	unsigned int VAO, VBO;

	// Vertex data
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), bgVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float)*3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float)*7));
	glEnableVertexArrayAttrib(VAO, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glEnableVertexArrayAttrib(VAO, 2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Indices data
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	#pragma endregion

	jisaacs::Shader shader = jisaacs::Shader("assets/shader.vert", "assets/shader.frag");
	shader.use();

	jisaacs::Texture2D bgTexture = jisaacs::Texture2D("assets/bg.png", GL_LINEAR, GL_REPEAT);
	bgTexture.Bind(GL_TEXTURE0);


	while (!glfwWindowShouldClose(window)) {
		// Inputs
		glfwPollEvents();

		// Update
		shader.use();

		// Draw
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // background color
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // triangles from indices
			
		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
