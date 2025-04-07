#version 460 core

out vec4 fragment_colour;

in vec3 col;

void main() {
  fragment_colour = vec4(col.x > 0.0f, col.y > 0.0f, col.z > 0.0f, 1.0f);
}
