#ifndef SHADER_H
#define SHADER_H

#include <epoxy/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  // the program ID
  unsigned int ID;

  // constructor reads and builds the shader
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  // use/activate the shader
  void use();

  // utility uniform functions
  void setBool(const std::string &name, bool value);
  void setInt(const std::string &name, int value);
  void setFloat(const std::string &name, float value);
  void setFloat(const std::string &name, float v1, float v2, float v3,
                float v4);

protected:
  int compileShader(const char *shaderSource, GLenum shaderType);
};

#endif // !SHADER_H
#define SHADER_H
