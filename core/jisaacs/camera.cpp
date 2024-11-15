// Adapted from caleb carreon
#include "camera.h"

using namespace pl;

Camera::Camera(float width, float height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void Camera::processInput(GLFWwindow* window)
{
	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		cameraSpeed *= 2.0f; // Double if sprinting
	}
	//Movement forward, strafe, and up
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	//Joseph Isaacs helped me make the Q and E relative
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) * cameraSpeed;
	}
	//https://stackoverflow.com/questions/52492426/glfw-switching-boolean-toggle
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabHeld) {
		perspective = !perspective; // Toggle perspective or orthographic
	}
	tabHeld = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
	}
}
void Camera::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		//Check if first mouse call
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		// Get change in mouse position
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		// Multiply offset by sensitivity
		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		// Change rotation angles
		yaw += xoffset;
		pitch += yoffset;
		// Make sure that you can't look up too far up or down
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
		// Change camera look direction
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}
void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {\
	// Get scroll
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 120.0f) fov = 120.0f;
}
void Camera::use(GLFWwindow* window) {
	processInput(window);
	/*The author generated this text in part with GPT-3, OpenAI’s large-scale language-generation
	model using the following prompt: "Help me refactor this use function". Upon generating draft language, the
	author reviewed, edited, and revised the language to their own liking and takes ultimate
	responsibility for this content.*/
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		camera->mouse_callback(window, xpos, ypos);
	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		camera->scroll_callback(window, xoffset, yoffset);
	});

	// Set the user pointer to this camera instance for the window
	glfwSetWindowUserPointer(window, this);
}
glm::mat4 Camera::projection(glm::mat4& projection) const {
	// Switch between projection and orthographic
	if (perspective) projection = glm::perspective(glm::radians(fov), ((float)SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 1000.0f);
	else projection = glm::ortho(-2.0f, 2.0f, -2.0f * ((float)SCREEN_HEIGHT / SCREEN_WIDTH), 2.0f * ((float)SCREEN_HEIGHT / SCREEN_WIDTH), 0.1f, 1000.0f);
	return projection;
}
glm::mat4 Camera::projection() const {
	glm::mat4 proj;
	Camera::projection(proj);
	return proj;
}
glm::mat4 Camera::viewLookAt(glm::mat4& view) const {
	// Set view position
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}
glm::mat4 Camera::viewLookAt() const {
	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}
void Camera::timeChange(float currentFrame) {
	// Change the time
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
glm::vec3 Camera::getPosition() {
	return cameraPos;
}