#pragma once


#include<iostream>
#include <filesystem>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <math.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


class Texture
{
public:
	GLuint texture;
	Texture(const char* _textureFile);
	~Texture();
};
