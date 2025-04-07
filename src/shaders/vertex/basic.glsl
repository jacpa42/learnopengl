#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 tex_coords;
out vec3 fragment_positon;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  // Calculate the world position of the vertex
  vec4 world_pos = model * vec4(aPos, 1.0);

  fragment_positon = aPos.xyz;
  tex_coords = aTexCoord;
  normal = normalize(mat3(transpose(inverse(model))) * aNormal);

  // Final vertex position after projection
  gl_Position = projection * view * world_pos;
}
