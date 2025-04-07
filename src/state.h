#pragma once

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>

struct Timer {
  double last_polled = glfwGetTime();
  /// Updates the last_polled=now and returns the time since the previous poll.
  double delta_time() {
    double now = glfwGetTime();
    double delta = now - last_polled;
    last_polled = now;
    return delta;
  }
};

struct LightSource {
  glm::vec3 pos;
  glm::vec3 col = glm::vec3(1.0f, 1.0f, 1.0f);
  float diffuse_strength = 0.8, specular_strength = 1.5;

  LightSource(glm::vec3 pos, glm::vec3 col)
      : pos(pos), col(glm::normalize(col)) {};

  constexpr glm::vec3 diffuse() { return diffuse_strength * col; }
  constexpr glm::vec3 specular() { return specular_strength * col; }
};

struct Camera {
  float speed = 2.5;
  float zoom = 45;
  float pitch = -31, yaw = 228;

  glm::vec3 pos;
  glm::vec3 up = glm::vec3(0, 1, 0);

  Camera(glm::vec3 pos) : pos(pos) {};

  glm::mat4 view() const { return glm::lookAt(pos, pos + dir(), up); }
  glm::mat4 projection(float screen_width, float screen_height) const {
    return glm::perspective(glm::radians(zoom), screen_width / screen_height,
                            0.1f, 100.0f);
  }
  glm::vec3 dir() const {
    return glm::normalize(
        glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                  sin(glm::radians(pitch)),
                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
  }
};

struct Mouse {
  bool in_frame = false;
  uint8_t left = 0;
  uint8_t right = 0;
  glm::vec2 pos = glm::vec2(0, 0);

  bool left_pressed() { return left & 1; }
  bool right_pressed() { return right & 1; }
};

struct State {
  unsigned int screen_width, screen_height;
  Mouse mouse;
  LightSource light;
  Camera camera = Camera(glm::vec3(1, 2, 3));

  Timer timer;

  glm::mat4 model = {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},

  };

  State(unsigned int width, unsigned int height, LightSource light,
        glm::vec3 camera_position)
      : screen_width(width), screen_height(height), light(light),
        camera(camera_position) {}
};
