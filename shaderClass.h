#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	char vertexFile;
	char fragmentFile;

	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

    void use();

    void setBool(const std::string& name, bool _value) const;

    void setInt(const std::string& name, int _value) const;

    void setFloat(const std::string& name, float _value) const;

    void setVec3(const std::string& name, glm::vec3 _value) const;

    void setMat4(const std::string& name, glm::mat4 _value) const;

	void setArray(const std::string& name, int count, float* _value) const;

private:
    GLint GetLocation(const std::string& name) const;

	void checkCompileErrors(unsigned int shader, std::string type);

};
#endif