// Joseph Isaacs
// 10/21/24

#include <stdio.h>
#include <iostream>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <jisaacs/shader.h>
#include <jisaacs/texture.h>
#include <jisaacs/camera.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

enum ProjectionType {
	ORTHOGRAPHIC = 0,
	PERSPECTIVE = 1
};
 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/*
glm::vec3 cameraPos = glm::vec3(-2.98126, 0.321037, 1.68709);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); 
*/

pl::Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float nearPlane = 0.1f, farPlane = 1000.0f; //- Near plane of 0.1, Far plane of 1000

const unsigned int CUBENUM = 20;
glm::vec3 cubePositions[CUBENUM];
glm::vec3 cubeScales[CUBENUM];
glm::vec3 cubeRotations[CUBENUM];
glm::vec3 cubeRotationDirs[CUBENUM];
float cubeRotationAngles[CUBENUM];
float cubeRotationSpeed = 0.7f;

const float cubeVertices[] = {
		// position				normal				uv
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
                                             
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
                                             
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
                                             
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
                                             
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
                                             
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
};

/*
const float speed = 2.5f, sprintSpeedMultiplier = 2.0f;
void movementInput(GLFWwindow* window);
//- Bonus +2: Add an option to toggle between Perspective and Orthographic projection.
// press [TAB] 
ProjectionType cameraMode = ProjectionType::PERSPECTIVE;
bool holdingCameraModeToggle = false;
void cameraProjectionInput(GLFWwindow* window);
void processInput(GLFWwindow* window);

//- Aiming with mouse
const float sensitivity = 0.1f;
float pitch, yaw = -90.0f; //- Camera starts looking looking along world Z axis.
float lastX = 0.0f, lastY = 0.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//- Mouse scroll to change FOV
float fov = 60.0f; //- Perspective projection with a default FOV of 60 degrees
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Cursor locking
bool cursorLocked = false;
void cursorLocking(GLFWwindow* window) {
	// if (ImGui::GetIO().WantCaptureMouse) { return; // Skip processing if ImGui is capturing the mouse
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Locks
		cursorLocked = true;
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //Unlocks
		cursorLocked = false;
	}
}
*/
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
		
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //Unlocks
	#pragma endregion
	#pragma region Geometry data
	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	#pragma endregion
	# pragma region World
	//- Cubes at random positions, rotations, and scales
	for (int i = 0; i < CUBENUM; i++) {
		float cubeBounds = 6.0f;
		float fieldStartsAt = -0.0f;
		cubePositions[i].x = ew::RandomRange(-cubeBounds, cubeBounds);
		cubePositions[i].y = ew::RandomRange(-cubeBounds, cubeBounds);
		cubePositions[i].z = ew::RandomRange(fieldStartsAt+cubeBounds/2.0f, fieldStartsAt-cubeBounds/2.0f);
		
		float cubeMinScale = 0.25f;
		float cubeMaxScale = 1.2f;
		cubeScales[i] = glm::vec3(ew::RandomRange(cubeMinScale, cubeMaxScale));

		float cubeRotationAngle = ew::RandomRange(0.0f, 3.0f);
		cubeRotations[i] = cubeRotationAngle * glm::vec3(ew::RandomRange(-1.0f, 1.0f), ew::RandomRange(-1.0f, 1.0f), ew::RandomRange(-1.0f, 1.0f));
		cubeRotationDirs[i] = glm::vec3(ew::RandomRange(-1.0f, 1.0f), ew::RandomRange(-1.0f, 1.0f), ew::RandomRange(-1.0f, 1.0f));
		cubeRotationAngles[i] = 0.0f;
	}
	# pragma endregion

	jisaacs::Shader brickShader = jisaacs::Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
	jisaacs::Shader lightShader = jisaacs::Shader("assets/shaders/light.vert", "assets/shaders/light.frag");
	jisaacs::Texture2D brick = jisaacs::Texture2D("assets/textures/brick.png", GL_LINEAR, GL_REPEAT);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float ambientK = 0.1f, specularK = 0.5f, diffuseK = 0.5f;
	float shininess = 18.0f;
	bool blinnPhong = true;

	camera.setPosition(glm::vec3(-2.98126, 0.321037, 1.68709));
	camera.setFront(glm::vec3(0.855282, -0.0662739, -0.513907));

	while (!glfwWindowShouldClose(window)) {
		// Inputs
		glfwPollEvents();

		// Update
		float time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;

		camera.use(window);
		camera.timeChange(time);

		glm::mat4 projection = camera.projection();
		glm::mat4 view = camera.viewLookAt();

		// Draw
		// background
		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// light box
		lightShader.use();
		glBindVertexArray(VAO);
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::rotate(lightModel, cubeRotationAngles[0], cubeRotations[0]);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		lightShader.setMat4("model", lightModel);
		lightShader.setVec3("lightPos", lightPos);
		lightShader.setVec3("lightColor", lightColor);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glEnable(GL_DEPTH_TEST);

		// boxes
		brickShader.use();
		brickShader.setVec3("viewPos", camera.getPosition());
		brickShader.setBool("blinnPhong", blinnPhong);
		brickShader.setVec3("lightPos", lightPos);
		brickShader.setVec3("lightColor", lightColor);
		brickShader.setFloat("ambientStrength", ambientK);
		brickShader.setFloat("diffuseStrength", diffuseK);
		brickShader.setFloat("specularStrength", specularK);
		brickShader.setFloat("shininess", shininess);
		brick.Bind(GL_TEXTURE0);
		glBindVertexArray(VAO);

		brickShader.setMat4("projection", projection);
		brickShader.setMat4("view", view);
		for (unsigned int i = 0; i < CUBENUM; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, cubeRotationAngles[i], cubeRotations[i]);
			model = glm::scale(model, cubeScales[i]);

			brickShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		#pragma region ImGui
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Lighting");
		ImGui::Checkbox("Blinn-Phong", &blinnPhong);
		ImGui::DragFloat3("Light Position", &lightPos.x, 0.1f);
		ImGui::ColorEdit3("Light Color", &lightColor.r);
		ImGui::SliderFloat("Ambient K", &ambientK, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse K", &diffuseK, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular K", &specularK, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &shininess, 2.0f, 1024.0f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		#pragma endregion

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

/*
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

void debugInputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		std::cout << "Camera Pos: ";
		std::cout << "(" << camera.getPosition().x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
		std::cout << "Camera Front: ";
		std::cout << "(" << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << ")" << std::endl;
	}
}


void processInput(GLFWwindow* window) {
	movementInput(window);
	cameraProjectionInput(window);
	cursorLocking(window);
	debugInputs(window);
}

bool wasCursorLocked;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (cursorLocked != wasCursorLocked)
	{
		lastX = xpos;
		lastY = ypos;
	}

	wasCursorLocked = cursorLocked;
	if (!cursorLocked) {
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
		return;
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
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;

	//- Clamp FOV between 1 and 120 degrees
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 120.0f)
		fov = 120.0f;
}
*/
