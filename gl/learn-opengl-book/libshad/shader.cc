#include "libshad/shader.hh"
#include <print>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;

  try {
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream vShaderStream, fShaderStream;

    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::println(stderr, "Error::Shader::FileNotSuccessfullyRead: {}\n",
                 e.what());
    throw e;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // 2. compile shaders
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // vertex shader
  vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
  if (vertex == -1) {
    throw std::runtime_error("Couldn't compile the vertex shader");
  }
  // fragment shader
  fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
  if (fragment == -1) {
    throw std::runtime_error("Couldn't compile the fragment shader");
  }

  // 3. link shaders
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  // print linking errors if any
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::println(stderr, "Error::Shader::Program::LinkingFailed\n{}\n",
                 infoLog);
  }

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float v1, float v2, float v3,
                      float v4) {
  glUniform4f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3, v4);
}

int Shader::compileShader(const char *shaderSource, GLenum shaderType) {
  // Compile the shader
  uint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  // Check for shader compilation errors
  int success;
  char infoLog[512];

  const char *shaderTypeStr =
      shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::println(stderr, "Error::Shader::{}::CompilationFailed\n{}\n",
                 shaderType, infoLog);
    return -1;
  } else {
    std::println("{} shader compiled successfully", shaderTypeStr);
  }
  return shader;
}
