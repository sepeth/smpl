#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <epoxy/gl.h>
#include <print>
#include <string_view>

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos, 1.0);\n"
    "  ourColor = aColor;\n"
    "}"; // why the \0?. Tried and found that it works without.

const char *fragmentShaderSource = "#version 330 core\n"
                                   "in vec3 ourColor;\n"
                                   "out vec4 FragColor;\n"
                                   "void main() {\n"
                                   "  FragColor = vec4(ourColor, 1.0);\n"
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

// clang-format off
float vertices[] = {
     0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,  // right
    -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  // left
     0.0f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f   // top
};
// clang-format on

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

  /* Print OpenGL version */
  std::println("OpenGL version: {}",
               std::string_view((char *)glGetString(GL_VERSION)));

  /* Set the viewport */
  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        std::println("Changing viewport size to {}x{}", width, height);
        glViewport(0, 0, width, height);
      });

  // Send the vertices to the GPU
  uint VBO; // used to have = 0 here, but deleted due to goto cleanup.
  glGenBuffers(1, &VBO);
  std::println("VBO: {}", VBO);
  // 0. Copy our vertices array in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Vertex Array Object (VAO)
  // This is a container object that stores all of the links between the
  // VBO(s) and attribute(s). Using a VAO means we have to set up the vertex
  // attribute pointers once, and then just bind the VAO every time we want to
  // draw the object.
  uint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // 1. Then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  // INTERESTING BIT: pointer is used as offset?
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Compile the shaders
  int vertexShader;
  vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
  if (vertexShader == -1) {
    goto cleanup;
  }
  int fragmentShader;
  fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
  if (fragmentShader == -1) {
    goto cleanup;
  }

  // Link the shaders and create a shader program
  uint shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::println(stderr, "Failed to link the shader program: {}", infoLog);
    goto cleanup;
  } else {
    std::println("Shader program linked successfully");
  }

  // Delete the shaders as they are linked to the shader program, and we no
  // longer need them (they are stored in the shader program)
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Query the uniform location in the fragment shader
  int vertexColorLocation;
  vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Handle inputs */
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the damn triangle
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    // now render the triangle
    glBindVertexArray(VAO);
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
