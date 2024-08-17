#define GLFW_INCLUDE_NONE
#include "cgfs.hh"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <print>
#include <string_view>

int Width{800};
int Height{600};
const char *Title{"LearnOpenGL"};

void processInput(GLFWwindow *window);
void PutPixel(int x, int y, Color color);

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
  GLFWwindow *window = glfwCreateWindow(Width, Height, Title, NULL, NULL);
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
  std::println("OpenGL version: {}", (char *)glGetString(GL_VERSION));

  /* Set the viewport */
  glViewport(0, 0, Width, Height);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        std::println("Changing viewport size to {}x{}", width, height);
        glViewport(0, 0, width, height);
        Width = width;
        Height = height;
      });

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Handle inputs */
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a pixel
    PutPixel(0, 0, {1.0f, 1.0f, 1.0f, 1.0f});

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

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}
