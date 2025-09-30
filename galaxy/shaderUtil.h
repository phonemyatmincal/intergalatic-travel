#pragma once

#include <string>
#include <glad/glad.h>

// Loads a shader source file and compiles it into a shader object.
unsigned int compileShader(const char* path, GLenum shaderType);

// Links a vertex and fragment shader into a shader program.
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);

// Utility function to read a file into a string.
std::string readFile(const char* filePath);
