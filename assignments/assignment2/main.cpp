// Joseph Isaacs
// 9/27/24

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

const float cubeVertices[] = {
	// position			 // color				  // texture coords
	-1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom left
	 1.0f, -1.0f, 0.0f,	 1.0f, 0.0f, 0.0f, 1.0f,  3.0f, 0.0f, // Bottom right
	 1.0f,  1.0f, 0.0f,	 1.0f, 1.0f, 0.0f, 1.0f,  3.0f, 2.531f, // Top right
	-1.0f,  1.0f, 0.0f,	 0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 2.531f, // Top left
};

const int cubeIndices[] = {
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
	// Background
	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 7));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	#pragma endregion

	jisaacs::Shader bgShader = jisaacs::Shader("assets/bgShader.vert", "assets/bgShader.frag");
	jisaacs::Shader guyShader = jisaacs::Shader("assets/guyShader.vert", "assets/guyShader.frag");

	jisaacs::Texture2D bgTexture = jisaacs::Texture2D("assets/bg.png", GL_LINEAR, GL_REPEAT);
	jisaacs::Texture2D guyTexture = jisaacs::Texture2D("assets/guy.png", GL_NEAREST, GL_CLAMP_TO_BORDER);

	while (!glfwWindowShouldClose(window)) {
		// Inputs
		glfwPollEvents();

		// Update
		float time = glfwGetTime();

		// Draw background
		glDisable(GL_BLEND);
		bgShader.use();
		bgShader.setInt("ourTexture", 0);
		bgTexture.Bind(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Draw character
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		guyShader.use();
		guyShader.setInt("ourTexture", 1);
		guyShader.setFloat("waveT", time);
		guyTexture.Bind(GL_TEXTURE1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
