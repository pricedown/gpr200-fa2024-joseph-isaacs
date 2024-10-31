#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include "../ew/external/glad.h"

namespace jisaacs {
	class Shader {
	public:
		unsigned int programID;
		Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
		// use/activate the shader
		void use();
		// utility uniform functions
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec3(const std::string& name, float v1, float v2, float v3) const;
		void setVec3(const std::string& name, glm::vec3) const;
		void setMat4(const std::string& name, const glm::mat4 mat) const;
	};
}
