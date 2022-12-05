#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#define STB_INCLUDE_LINE_GLSL
#define STB_INCLUDE_IMPLEMENTATION
#include <stb/stb_include.h>

//Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type) const {
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if(!file){
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCStr = sourceString.c_str();
    file.close();
    // //TODO: Complete this function               [DONE]
    // //Note: The function "checkForShaderCompilationErrors" checks if there is
    // // an error in the given shader. You should use it to check if there is a
    // // compilation error and print it so that you can know what is wrong with
    // // the shader. The returned string will be empty if there is no errors.
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &sourceCStr, nullptr); //Send shader source code
    glCompileShader(shaderID); //Compile the shader code
    // free(source);
    

    std::string Error = checkForShaderCompilationErrors(shaderID);
    if(Error != "")
    {
        std::cout << "Error: " << Error << std::endl;
        return false;
    }
    //We return true if the compilation succeeded
    glAttachShader(program, shaderID); //Attach shader to program
    glDeleteShader(shaderID); //Delete shader (the shader is already attached to the program so its object is no longer needed)
    return true;
}



bool our::ShaderProgram::link() const {
    //TODO: Complete this function              [DONE]
    //Note: The function "checkForLinkingErrors" checks if there is
    // an error in the given program. You should use it to check if there is a
    // linking error and print it so that you can know what is wrong with the
    // program. The returned string will be empty if there is no errors.
    glLinkProgram(program);

    std::string Error = checkForLinkingErrors(program);
    if(Error != "")
    {
        std::cerr << "Error: " << Error << std::endl;
        return false;
    }
    //We return true if the compilation succeeded
    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader){
     //Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program){
     //Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}