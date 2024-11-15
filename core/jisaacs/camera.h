// Adapted from caleb carreon
#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "../ew/external/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace pl {
	class Camera {
	public:
		Camera(float width, float height);
		void use(GLFWwindow* window);
		void processInput(GLFWwindow* window);
		void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		glm::mat4 projection(glm::mat4& projection) const;
		glm::mat4 projection() const;
		glm::mat4 viewLookAt(glm::mat4& view) const;
		glm::mat4 viewLookAt() const;
		void timeChange(float currentFrame);
		glm::vec3 getPosition();
	private:
		//Camera
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		bool perspective = true;
		bool tabHeld = false;
		//Timing
		float deltaTime = 0.0f;	// Time between current frame and last frame
		float lastFrame = 0.0f; // Time of last frame
		//Mouse
		float lastX = 400;
		float lastY = 300;
		//Rotation
		float yaw = -90.0f;
		float pitch = 0.0f;
		bool firstMouse = true;
		//FOV
		float fov = 45.0f;
		//Screen 
		float SCREEN_WIDTH;
		float SCREEN_HEIGHT;
	};
#endif
}
