#define LOG_LEVEL LL_INFO

#include "glad/include/glad/glad.h"
#include "log/log.h"
#include "model/model.h"
#include "shaders/shader.cpp"
#include "state.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, State &state);
void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);
void mouse_enter_callback(GLFWwindow *window, int entered);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

/// Global application state variable
State state(800, 600,
            LightSource(glm::vec3(1.2, 1.0, 2.0), glm::vec3(1.0, 1.0, 1.0)),
            glm::vec3(10, 10, 10));

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(state.screen_width, state.screen_height, "", NULL, NULL);
  if (window == NULL) {
    ERROR("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // capture mouse and hide it
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetCursorEnterCallback(window, mouse_enter_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    ERROR("Failed to initialize GLAD");
    glfwTerminate();
    return -1;
  }

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  // Enable wireframe
  /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

  Shader model_shader, compass_shader;

  // Create the shaders
  {
    std::optional<Shader> shader_opt =
        Shader::create("model_shader", "src/shaders/vertex/basic.glsl",
                       "src/shaders/fragment/basic.glsl");

    if (!shader_opt.has_value()) {
      ERROR("Failed to construct shader. Exiting");
      return -1;
    }
    model_shader = std::move(*shader_opt);

    shader_opt =
        Shader::create("compass_shader", "src/shaders/vertex/compass.glsl",
                       "src/shaders/fragment/compass.glsl");

    if (!shader_opt.has_value()) {
      ERROR("Failed to construct shader. Exiting");
      return -1;
    }
    compass_shader = std::move(*shader_opt);
  }

  Model cacodemon("cacodemon");

  {
    std::optional<Model> model_opt = Model::load(
        "src/model/assets/doom-eternal-cacodemon/cacodemon_LOD0.obj");

    if (model_opt.has_value()) {
      cacodemon = *model_opt;
    } else {
      ERROR("Failed to load {}!", cacodemon.name);
      return -1;
    }
  }

  while (!glfwWindowShouldClose(window)) {
    state.camera.speed = state.timer.delta_time() * 5;
    processInput(window, state);

    // clear screen
    {
      glm::vec3 clear = glm::vec3(237, 135, 150) / 255.0f;
      clear *= 0.2;
      glClearColor(clear.r, clear.g, clear.b, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // model draw
    {
      model_shader.use();

      auto model = glm::scale(state.model, glm::vec3(0.03));
      model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
      model = glm::translate(model, glm::vec3(-100, 0, 0));
      model_shader.set_mat4("model", model);
      model_shader.set_mat4("view", state.camera.view());
      model_shader.set_mat4(
          "projection",
          state.camera.projection(state.screen_width, state.screen_height));

      model_shader.set_vec3("camera_position", state.camera.pos);
      model_shader.set_vec3("light.pos", state.light.pos);
      model_shader.set_vec3("light.col", state.light.col);

      cacodemon.draw(model_shader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window, State &state) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // Track mouse button state
  {
    state.mouse.left <<= 1;
    state.mouse.right <<= 1;

    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      state.mouse.left |= 1;

    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
      state.mouse.right |= 1;
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    state.camera.pos += state.camera.speed * state.camera.dir();
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    state.camera.pos -= state.camera.speed * state.camera.dir();
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    state.camera.pos -=
        glm::normalize(glm::cross(state.camera.dir(), state.camera.up)) *
        state.camera.speed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    state.camera.pos +=
        glm::normalize(glm::cross(state.camera.dir(), state.camera.up)) *
        state.camera.speed;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    state.camera.pos += state.camera.speed * glm::normalize(state.camera.up);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  state.screen_width = width;
  state.screen_height = height;
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
  glm::vec2 new_pos = glm::vec2(x_pos, y_pos);
  float sensitivity = 0.1;
  glm::vec2 delta = sensitivity * (state.mouse.pos - new_pos);

  state.camera.yaw += -delta.x;
  state.camera.pitch = glm::clamp(state.camera.pitch + delta.y, -89.0f, 89.0f);
  state.mouse.pos = new_pos;
}

void mouse_enter_callback(GLFWwindow *window, int entered) {
  state.mouse.in_frame = (bool)entered;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  state.camera.zoom =
      glm::clamp(state.camera.zoom - (float)yoffset, 1.0f, 45.0f);
}
