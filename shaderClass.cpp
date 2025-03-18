#include"shaderClass.h"


// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile; // if => input file
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {

        vShaderFile.open(vertexFile);
        fShaderFile.open(fragmentFile);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        //std::cout << vertexFile << "=============" << std::endl;
        //std::cout << vertexCode << std::endl;
        //std::cout << fragmentFile << "============="  << std::endl;
        //std::cout << fragmentCode << std::endl;
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    // this is c string because OpenGL is c thing
    const char* vShaderCode_c_str = vertexCode.c_str();
    const char* fShaderCode_c_str = fragmentCode.c_str();

    // compile shader
    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode_c_str, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode_c_str, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // ====================

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool _value) const
{
	glUniform1i(GetLocation(name), _value);
}
void Shader::setInt(const std::string& name, int _value) const
{
	glUniform1i(GetLocation(name), _value);
}
void Shader::setFloat(const std::string& name, float _value) const
{
	glUniform1f(GetLocation(name), _value);
}
void Shader::setVec3(const std::string& name, glm::vec3 _value) const
{
	glUniform3fv(GetLocation(name), 1, glm::value_ptr(_value));
}

void Shader::setMat4(const std::string& name, glm::mat4 _value) const
{
	glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(_value));
}

void Shader::setArray(const std::string& name, int count, float* _value) const {
	glUniform1fv(GetLocation(name), count, _value);
}

GLint Shader::GetLocation(const std::string& name) const
{
	return glGetUniformLocation(ID, name.c_str());
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}