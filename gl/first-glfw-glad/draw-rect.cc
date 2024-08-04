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

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
     0.5f,  0.5f, 0.0f, // top right
     0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f, // top left
};
// clang-format on

uint indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
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

  /* Set the viewport */
  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        std::println("Changing viewport size to {}x{}", width, height);
        glViewport(0, 0, width, height);
      });

  // 1. Bind Vertex Array Object
  // Vertex Array Object (VAO)
  // This is a container object that stores all of the links between the
  // VBO(s) and attribute(s). Using a VAO means we have to set up the vertex
  // attribute pointers once, and then just bind the VAO every time we want to
  // draw the object.
  uint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  std::println("Vertex Array Object (VAO): {}", VAO);

  // 2. Send the vertices to the GPU
  uint VBO; // used to have = 0 here, but deleted due to goto cleanup.
  glGenBuffers(1, &VBO);
  std::println("Vertex Buffer Object (VBO): {}", VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO); // IMPORTANT! After we bind VAO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /***
   * THIS EBO SECTION IS DIFFERENT THAN THE FIRST EXAMPLE
   ***/

  // 3. Send the indices to the GPU
  uint EBO;
  glGenBuffers(1, &EBO);
  std::println("Element Buffer Object (EBO): {}", EBO);
  // IMPORTANT! This is bound after VAO, which also keeps track of the EBO,
  // which means that when we bind the VAO back, it will remember the EBO as
  // well.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  /** Each vertex attribute takes its data from memory managed by a VBO and
   which VBO it takes its data from (you can have multiple VBOs) is determined
   by the VBO currently bound to GL_ARRAY_BUFFER when calling
   glVertexAttribPointer. Since the previously defined VBO is still bound before
   calling glVertexAttribPointer vertex attribute 0 is now associated with its
   vertex data. */
  // 2. Then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        /* offset */ (void *)0);
  glEnableVertexAttribArray(0);

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

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Handle inputs */
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, argc > 1 ? GL_LINE : GL_FILL);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // DIFFERENT THAN THE FIRST EXAMPLE: glDrawArrays -> glDrawElements
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // No longer needed because
    // we bound VAO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // Unbind VAO

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
