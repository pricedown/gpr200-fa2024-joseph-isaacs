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
const int SCREEN_HEIGHT = 600;

enum ProjectionType {
	ORTHOGRAPHIC = 0,
	PERSPECTIVE = 1
};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

ProjectionType cameraMode = ProjectionType::PERSPECTIVE;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); //- Camera starts looking looking along world Z axis.

float pitch, yaw;
float nearPlane = 0.1f, farPlane = 1000.0f; //- Near plane of 0.1, Far plane of 1000
const float walkSpeed = 2.5f, runSpeed = 5.0f;

float lastX = 0, lastY = 0;
const float sensitivity = 0.1f;
float fov = 60.0f; //- Perspective projection with a default FOV of 60 degrees
bool firstMouse = true;

const float cubeVertices[] = {
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
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

const unsigned int cubeIndices[] = {
	0, 1, 2, 
	2, 3, 0, 
};

const unsigned int CUBENUM = 20;
glm::vec3 cubePositions[CUBENUM];
glm::vec3 cubeRotationDirections[CUBENUM];

void movementInput(GLFWwindow* window) {
	float cameraSpeed = deltaTime; //- All movement must be scaled by deltaTime to be framerate independent
	
	//- Left Shift to Sprint(double base movement speed if held)*
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
		cameraSpeed *= 5.0f;
	else
		cameraSpeed *= 2.5f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, UP)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, UP)) * cameraSpeed;

	//- Q, E to move down / up along local Y axis, respectively*
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(glm::cross(cameraFront, UP), cameraFront)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(glm::cross(cameraFront, UP), cameraFront)) * cameraSpeed;
}

bool holding = false;
void processInput(GLFWwindow* window)
{
	movementInput(window);
	
	//- Bonus +2: Add an option to toggle between Perspective and Orthographic projection.
	// [TAB] to change mode
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (holding == false) {
			cameraMode = (ProjectionType)((cameraMode + 1) % 2);
		}
		holding = true;
	}
	else {
		holding = false;
	}
}

//- Aiming with mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//- Pitch is clamped to - 89 and 89 degrees
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

//- Mouse scroll to change FOV
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;

	//- Clamp FOV between 1 and 120 degrees
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 120.0f)
		fov = 120.0f;
}

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	#pragma endregion

	for (int i = 0; i < CUBENUM; i++) {
		float maxDist = 5.f;
		cubePositions[i].x = ew::RandomRange(-maxDist, maxDist);
		cubePositions[i].y = ew::RandomRange(-maxDist, maxDist);
		cubePositions[i].z = ew::RandomRange(-maxDist, maxDist);
		cubeRotationDirections[i] = glm::vec3(ew::RandomRange(-1.0f, 1.0f), ew::RandomRange(-1.0f, 1.0f), ew::RandomRange(-1.0f, 1.0f));
	}

	jisaacs::Shader shader = jisaacs::Shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");
	jisaacs::Texture2D brick = jisaacs::Texture2D("assets/textures/brick.png", GL_LINEAR, GL_REPEAT);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	while (!glfwWindowShouldClose(window)) {
		// Inputs
		glfwPollEvents();
		processInput(window);

		// Update
		float time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;

		glm::mat4 model = glm::mat4(1.0);

		glm::mat4 view;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, UP);

		glm::mat4 projection = glm::mat4(1.0);
		if (cameraMode == ProjectionType::PERSPECTIVE) {
			projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH/SCREEN_HEIGHT, nearPlane, farPlane);  // FOV accounts for the screen's aspect ratio
		}
		else if (cameraMode == ProjectionType::ORTHOGRAPHIC) {
			projection = glm::ortho(-SCREEN_WIDTH / 400.0f, SCREEN_WIDTH / 400.0f, -SCREEN_HEIGHT / 400.0f, SCREEN_HEIGHT / 400.0f, nearPlane, farPlane);
		}
		
		// Draw

		// background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// brick square
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		brick.Bind(GL_TEXTURE0);

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < CUBENUM; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 7.0f * (i+1) * time;
			model = glm::rotate(model, glm::radians(angle), cubeRotationDirections[i]);
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
