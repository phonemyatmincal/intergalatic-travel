#include "shaderUtil.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

std::string readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
    } else {
        std::cerr << "Failed to read shader file: " << filePath << "\n";
    }
    return buffer.str();
}

unsigned int compileShader(const char* path, GLenum shaderType) {
    std::string sourceStr = readFile(path);
    const char* source = sourceStr.c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Error handling
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed (" << path << "):\n" << infoLog << "\n";
    }

    return shader;
}

unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    unsigned int vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Error handling
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
