#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <print>
#include <string_view>

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}"; // why the \0?. Tried and found that it works without.

const char *fragmentShaderSource1 =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

const char *fragmentShaderSource2 =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "  FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
    "}";

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

int compileShader(const char *shaderSource, GLenum shaderType) {
  // Compile the shader
  uint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  // Check for shader compilation errors
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::println(stderr, "Failed to compile the vertex shader: {}", infoLog);
    return -1;
  } else {
    const char *shaderTypeStr =
        shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
    std::println("{} shader compiled successfully", shaderTypeStr);
  }
  return shader;
}

int createShaderProgram(uint vertexShader, uint fragmentShader) {
  uint prog;
  prog = glCreateProgram();
  glAttachShader(prog, vertexShader);
  glAttachShader(prog, fragmentShader);
  glLinkProgram(prog);
  int success;
  char infoLog[512];
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(prog, 512, NULL, infoLog);
    std::println(stderr, "Failed to link the shader program: {}", infoLog);
    return -1;
  } else {
    std::println("Shader program linked successfully");
  }
  return prog;
}

float triangle1[] = {
    -0.6f, -0.5f, 0.0f, // left
    0.4f,  -0.5f, 0.0f, // right
    -0.1f, 0.5f,  0.0f, // top
};

float triangle2[] = {
    0.1f, 0.5f,  0.0f, // left
    0.5f, 0.5f,  0.0f, // right
    0.6f, -0.5f, 0.0f, // bottom
};

int main(int argc, char *argv[]) {
  /* Initialize the library */
  if (!glfwInit()) {
    std::println(stderr, "Failed to initialize GLFW\n");
    return -1;
  } else {
    std::println("GLFW initialized");
  }
  int exitCode = -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // The book suggest to add this line to make it work on Mac, but worked on
  // Sonoma without it as well
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  /* Create a windowed mode window and its OpenGL context */
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (!window) {
    std::println(stderr, "Failed to create window");
    goto cleanup;
  } else {
    std::println("Window created");
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println(stderr, "Failed to initialize GLAD");
    goto cleanup;
  } else {
    std::println("GLAD initialized");
  }

  /* Print OpenGL version */
  std::println("OpenGL version: {}",
               std::string_view((char *)glGetString(GL_VERSION)));

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        std::println("Changing viewport size to {}x{}", width, height);
        glViewport(0, 0, width, height);
      });

  // Vertex Array Object (VAO)
  // This is a container object that stores all of the links between the
  // VBO(s) and attribute(s). Using a VAO means we have to set up the vertex
  // attribute pointers once, and then just bind the VAO every time we want to
  // draw the object.
  uint VAO1;
  glGenVertexArrays(1, &VAO1);

  glBindVertexArray(VAO1);
  // Send the vertices to the GPU
  uint VBO1; // used to have = 0 here, but deleted due to goto cleanup.
  glGenBuffers(1, &VBO1);
  // 0. Copy our vertices array in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);

  // 1. Then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Second VAO
  uint VAO2;
  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);
  uint VBO2;
  glGenBuffers(1, &VBO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Compile the shaders
  int vertexShader;
  vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
  if (vertexShader == -1) {
    goto cleanup;
  }
  int fragmentShader1;
  fragmentShader1 = compileShader(fragmentShaderSource1, GL_FRAGMENT_SHADER);
  if (fragmentShader1 == -1) {
    goto cleanup;
  }
  int fragmentShader2;
  fragmentShader2 = compileShader(fragmentShaderSource2, GL_FRAGMENT_SHADER);
  if (fragmentShader2 == -1) {
    goto cleanup;
  }

  // Link the shaders and create a shader program
  uint shaderProgram1;
  shaderProgram1 = createShaderProgram(vertexShader, fragmentShader1);
  if (shaderProgram1 == -1) {
    goto cleanup;
  }

  uint shaderProgram2;
  shaderProgram2 = createShaderProgram(vertexShader, fragmentShader2);
  if (shaderProgram2 == -1) {
    goto cleanup;
  }

  // Delete the shaders as they are linked to the shader program, and we no
  // longer need them (they are stored in the shader program)
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader1);
  glDeleteShader(fragmentShader2);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Handle inputs */
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the damn triangle
    glUseProgram(shaderProgram1);
    glBindVertexArray(VAO1);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(shaderProgram2);
    glBindVertexArray(VAO2);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  exitCode = 0;

cleanup:
  glfwTerminate();
  return exitCode;
}
