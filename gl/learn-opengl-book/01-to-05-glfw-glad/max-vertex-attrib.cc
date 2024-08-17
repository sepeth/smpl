#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <print>
#include <string_view>

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

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

  int nrVertexAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrVertexAttributes);
  std::println("Maximum nr of vertex attributes supported: {}",
               nrVertexAttributes);

  exitCode = 0;

cleanup:
  glfwTerminate();
  return exitCode;
}
