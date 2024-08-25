#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aFaceTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec2 FaceTexCoord;

void main() {
  gl_Position = vec4(aPos, 1.0);
  ourColor = aColor;
  TexCoord = aTexCoord;
  FaceTexCoord = aFaceTexCoord;
}
