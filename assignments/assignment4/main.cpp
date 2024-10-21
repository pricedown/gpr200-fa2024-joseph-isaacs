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

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); 
const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float nearPlane = 0.1f, farPlane = 1000.0f; //- Near plane of 0.1, Far plane of 1000

const unsigned int CUBENUM = 20;
glm::vec3 cubePositions[CUBENUM];
glm::vec3 cubeRotationDirections[CUBENUM];
glm::vec3 cubeScales[CUBENUM];
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

const float speed = 2.5f, sprintSpeedMultiplier = 2.0f;
void movementInput(GLFWwindow* window) {
	float cameraSpeed = speed * deltaTime; //- All movement must be scaled by deltaTime to be framerate independent
	
	//- Left Shift to Sprint(double base movement speed if held)*
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
		cameraSpeed *= sprintSpeedMultiplier;

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

//- Bonus +2: Add an option to toggle between Perspective and Orthographic projection.
// press [TAB] 
ProjectionType cameraMode = ProjectionType::PERSPECTIVE;
bool holdingCameraModeToggle = false;
void cameraProjectionInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (holdingCameraModeToggle == false) {
			cameraMode = (ProjectionType)((cameraMode + 1) % 2);
		}
		holdingCameraModeToggle = true;
	}
	else {
		holdingCameraModeToggle = false;
	}
}

void processInput(GLFWwindow* window)
{
	movementInput(window);
	cameraProjectionInput(window);
}

//- Aiming with mouse
const float sensitivity = 0.1f;
float pitch, yaw = -90.0f; //- Camera starts looking looking along world Z axis.
float lastX = 0.0f, lastY = 0.0f;
bool firstMouse = true;
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
float fov = 60.0f; //- Perspective projection with a default FOV of 60 degrees
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
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	#pragma endregion
	
	// Cubes random positions, rotations, and scales
	for (int i = 0; i < CUBENUM; i++) {
		float cubeBounds = 6.0f;
		float fieldStartsAt = -4.0f;
		cubePositions[i].x = ew::RandomRange(-cubeBounds, cubeBounds);
		cubePositions[i].y = ew::RandomRange(-cubeBounds, cubeBounds);
		cubePositions[i].z = ew::RandomRange(fieldStartsAt, fieldStartsAt-cubeBounds*2);
		
		float cubeMinScale = 0.25f;
		float cubeMaxScale = 1.2f;
		cubeScales[i] = glm::vec3(ew::RandomRange(cubeMinScale, cubeMaxScale));
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

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, UP); 

		glm::mat4 projection = glm::mat4(1.0);
		if (cameraMode == ProjectionType::PERSPECTIVE)
			projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH/SCREEN_HEIGHT, nearPlane, farPlane);  //- FOV accounts for the screen's aspect ratio
		else 
			projection = glm::ortho(-SCREEN_WIDTH / 300.0f, SCREEN_WIDTH / 300.0f, -SCREEN_HEIGHT / 300.0f, SCREEN_HEIGHT / 300.0f, nearPlane, farPlane);
		
		// Draw

		// background
		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// brick cubes
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		shader.use();
		brick.Bind(GL_TEXTURE0);
		glBindVertexArray(VAO);

		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		for (unsigned int i = 0; i < CUBENUM; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, cubePositions[i]);
			float angle = 7.0f * (i+1) * time; // BONUS +2: Animate your cubes by changing position, rotation, and/or scale every frame. This movement must be scaled with deltaTime.
			model = glm::rotate(model, glm::radians(angle), cubeRotationDirections[i]);
			model = glm::scale(model, cubeScales[i]);

			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
