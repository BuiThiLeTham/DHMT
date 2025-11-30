// sea.vert
#version 330 core
in vec3 position;
in vec3 wave;

uniform mat4 transformationMatrix;
uniform float time;

void main() {
  // Mặt đất phẳng - không có sóng
  // Tắt wave animation để tạo mặt đất
  gl_Position = transformationMatrix * vec4(position.x, position.y, position.z, 1.0);
}
