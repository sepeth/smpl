#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <libshad/shader.hh>
#include <print>
#include <string_view>

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

// clang-format off
float vertices[] = {
     0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,  // right
    -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  // left
     0.0f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f   // top
};
// clang-format on

void die(const std::string &msg, int code = 1) {
  std::println(stderr, "{}", msg);
  glfwTerminate();
  exit(code);
}

int main(int argc, char *argv[]) {
  /* Initialize the library */
  if (!glfwInit()) {
    die("Failed to initialize GLFW");
    return -1;
  } else {
    std::println("GLFW initialized");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // The book suggest to add this line to make it work on Mac, but worked on
  // Sonoma without it as well
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  /* Create a windowed mode window and its OpenGL context */
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (!window) {
    die("Failed to create window");
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

  if (!std::filesystem::exists("vertex.glsl")) {
    die("vertex.glsl not found");
  }
  if (!std::filesystem::exists("fragment.glsl")) {
    die("fragment.glsl not found");
  }
  Shader shader{"vertex.glsl", "fragment.glsl"};

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
    shader.use();
    shader.setFloat("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

    // now render the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  return 0;
}
