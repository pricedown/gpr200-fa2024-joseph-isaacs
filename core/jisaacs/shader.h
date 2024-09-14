#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../ew/external/glad.h"

namespace jisaacs {
	class Shader {
		unsigned int programID;
		Shader(const char* vertexPath, const char* fragmentPath);
		// use/activate the shader
		void use();
		// utility uniform functions
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
	};
}
