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

template <typename... Args>
void die(int exitCode, const std::format_string<Args...> &fmt, Args &&...args) {
  std::cerr << std::vformat(fmt.get(), std::make_format_args(args...))
            << std::endl;
  glfwTerminate();
  exit(exitCode);
}

template <typename... Args>
void die(const std::format_string<Args...> &fmt, Args &&...args) {
  die(1, fmt, args...);
}

unsigned int genTexture2D(unsigned char *data, int width, int height,
                          GLint format = GL_RGB, GLint wrapS = GL_REPEAT,
                          GLint wrapT = GL_REPEAT, GLint minFilter = GL_LINEAR,
                          GLint magFilter = GL_LINEAR) {
  unsigned int texture;

  glGenTextures(/*howmanytextures*/ 1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

  glTexImage2D(GL_TEXTURE_2D, 0, /*internalFormat*/ format, width, height,
               /*legacystuff*/ 0, format, GL_UNSIGNED_BYTE, data);

  // Automatically generate all the required mipmaps for the currently bound
  // texture
  glGenerateMipmap(GL_TEXTURE_2D);

  return texture;
}

unsigned char *loadImageOrDie(const char *filePath, int *width, int *height) {
  int _nrChannels;
  unsigned char *data = stbi_load(filePath, width, height, &_nrChannels, 0);
  if (!data) {
    die("Failed to load the texture file: {}", filePath);
  }
  return data;
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
  if (!std::filesystem::exists("fragment_combined.glsl")) {
    die("fragment_combined.glsl not found");
  }
  Shader shader{"vertex.glsl", "fragment_combined.glsl"};

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

  // Load the first texture
  int width, height;
  // tell stb_image.h to flip loaded texture's on the y-axis.
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = loadImageOrDie("container.jpg", &width, &height);

  int texture1 = genTexture2D(data, width, height);
  // now that we have the texture, we can free the image data
  stbi_image_free(data);

  // Load the second texture
  data = loadImageOrDie("awesomeface.png", &width, &height);
  int texture2 = genTexture2D(data, width, height, GL_RGBA);
  stbi_image_free(data);

  shader.use();
  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Handle inputs */
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // now render the triangle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /* Swap front and back buffers and poll IO events */
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();

  return 0;
}
