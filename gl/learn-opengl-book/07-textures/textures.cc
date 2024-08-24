#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <libshad/shader.hh>
#include <print>
#include <string_view>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

// clang-format off
float vertices[] = {
    // positions           // colors           // texture coords
     0.5f,  0.5f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // top right
     0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // bottom right
    -0.5f, -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // bottom left
    -0.5f,  0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f   // top left
};

unsigned int indices[] = {
  0, 1, 3, // first triangle
  1, 2, 3  // second triangle
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

  if (!std::filesystem::exists("vertex.glsl")) {
    die("vertex.glsl not found");
  }
  if (!std::filesystem::exists("fragment.glsl")) {
    die("fragment.glsl not found");
  }
  Shader shader{"vertex.glsl", "fragment.glsl"};

  /* Set the viewport */
  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        std::print("\x1B[2K\rChanging viewport size to {}x{}", width, height);
        glViewport(0, 0, width, height);
      });

  // Vertex Array Object (VAO)
  // This is a container object that stores all of the links between the
  // VBO(s) and attribute(s). Using a VAO means we have to set up the vertex
  // attribute pointers once, and then just bind the VAO every time we want to
  // draw the object.
  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // 1. Then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  // INTERESTING BIT: pointer is used as offset?
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  // Load the texture
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("container.jpg", &width, &height, &nrChannels, 0);
  if (!data) {
    die("Failed to load texture file");
  }

  unsigned int texture;
  glGenTextures(/*howmanytextures*/ 1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, /*legacystuff*/ 0,
               GL_RGB, GL_UNSIGNED_BYTE, data);
  // Automatically generate all the required mipmaps for the currently bound
  // texture
  glGenerateMipmap(GL_TEXTURE_2D);

  // now that we have the texture, we can free the image data
  stbi_image_free(data);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Handle inputs */
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // now render the triangle
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();

  return 0;
}
