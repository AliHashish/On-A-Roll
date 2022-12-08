#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>    // include to use map for cache

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram {

    private:
        //Shader Program Handle (OpenGL object name)
        GLuint program;

        std::map<std::string, GLuint> uniform_location_cache;   // a map to cache things in, instead of
                                                                // searching everytime for it

    public:
        ShaderProgram(){
            //TODO: (Req 1) Create A shader program     [DONE]
            program = glCreateProgram();
        }
        ~ShaderProgram(){
            //TODO: (Req 1) Delete a shader program     [DONE]
            // if program equals 0, then there isn't a program to delete
            if(program != 0) glDeleteProgram(program);  // else, delete the program
            program = 0;    // then set program = 0 again
        }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use() { 
            glUseProgram(program);
        }

        GLuint getUniformLocation(const std::string &name) {
            //TODO: (Req 1) Return the location of the uniform with the given name      [DONE]
            // return glGetUniformLocation(program, name);      // without using cache

            // We will cache the location in a map so that reusing it would not take as 
            // much time
            
            auto it = uniform_location_cache.find(name);    // searches for the uniform in our cache
            if(it != uniform_location_cache.end())
            {
                return it->second;
            }

            uniform_location_cache[name] = glGetUniformLocation(program, name.c_str()); // in case it was not found, then
                                                                                        // retrieve it and cache it
            return uniform_location_cache[name];
        }

        void set(const std::string &uniform, GLfloat value) {
            //TODO: (Req 1) Send the given float value to the given uniform     [DONE]
            glUniform1f(getUniformLocation(uniform), value);
            // 1f means one float value
        }

        void set(const std::string &uniform, GLuint value) {
            //TODO: (Req 1) Send the given unsigned integer value to the given uniform      [DONE]
            glUniform1ui(getUniformLocation(uniform), value);
            // 1ui means one unsigned integer value
        }

        void set(const std::string &uniform, GLint value) {
            //TODO: (Req 1) Send the given integer value to the given uniform       [DONE]
            glUniform1i(getUniformLocation(uniform), value);
            // 1i means one integer value
        }

        void set(const std::string &uniform, glm::vec2 value) {
            //TODO: (Req 1) Send the given 2D vector value to the given uniform     [DONE]
            glUniform2f(getUniformLocation(uniform), value.x, value.y);
            // 2f means 2 float values
        }

        void set(const std::string &uniform, glm::vec3 value) {
            //TODO: (Req 1) Send the given 3D vector value to the given uniform     [DONE]
            glUniform3f(getUniformLocation(uniform), value.x, value.y, value.z);
            // 3f means 3 float values
        }

        void set(const std::string &uniform, glm::vec4 value) {
            //TODO: (Req 1) Send the given 4D vector value to the given uniform     [DONE]
            glUniform4f(getUniformLocation(uniform), value.x, value.y, value.z, value.w);
            // 4f means 4 float values
        }

        void set(const std::string &uniform, glm::mat4 matrix) {
            //TODO: (Req 1) Send the given matrix 4x4 value to the given uniform    [DONE] 
            GLboolean transpose = false;
            glUniformMatrix4fv(getUniformLocation(uniform), 1, transpose, glm::value_ptr(matrix));
        }

        //TODO: (Req 1) Delete the copy constructor and assignment operator.        [DONE]
        //Question: Why do we delete the copy constructor and assignment operator?
        
        // Answer: 
        // It is important, so as to release the resources locked by the shader program class
        // This will also prevents problems such as if one object was deleted, and the other 
        // remains, the other will behave as if it still exists, while in fact it was deleted.
        ShaderProgram(ShaderProgram const &) = delete;
        ShaderProgram &operator=(ShaderProgram const &) = delete;
    };

}

#endif