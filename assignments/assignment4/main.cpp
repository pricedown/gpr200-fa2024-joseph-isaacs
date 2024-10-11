// Joseph Isaacs
// 9/27/24

#include <stdio.h>
#include <iostream>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <jisaacs/shader.h>
#include <jisaacs/texture.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const float quadVertices[] = {
	// position			 // color				  // texture coords
	-1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom left
	 1.0f, -1.0f, 0.0f,	 1.0f, 0.0f, 0.0f, 1.0f,  3.0f, 0.0f, // Bottom right
	 1.0f,  1.0f, 0.0f,	 1.0f, 1.0f, 0.0f, 1.0f,  3.0f, 3.0f, // Top right
	-1.0f,  1.0f, 0.0f,	 0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 3.0f, // Top left
};

const int quadIndices[] = {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 7));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	#pragma endregion

	jisaacs::Shader shader = jisaacs::Shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");
	jisaacs::Texture2D brick = jisaacs::Texture2D("assets/textures/brick.png", GL_LINEAR, GL_REPEAT);

	while (!glfwWindowShouldClose(window)) {
		// Inputs
		glfwPollEvents();

		// Update
		float time = glfwGetTime();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.5, 0.5, 0.5));
		model = glm::rotate(model, time, glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

		// Draw
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// brick square
		glEnable(GL_BLEND);
		shader.use();
		shader.setMat4("transform", model);
		brick.Bind(GL_TEXTURE0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
