#include "shader.h"

#include "../glad/include/glad/glad.h"
#include "../log/log.h"
#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <string>

Shader::Shader() {}

/// Attempts to load, compile and link the shader with its fragment and vertex
/// parts.
std::optional<Shader> Shader::create(const char *shader_name,
                                     const char *vertex_shader_path,
                                     const char *fragment_shader_path) {
  Shader shader(shader_name);
  int vertex_shader, fragment_shader;

  // Create the fragment shader
  {
    std::ifstream fragment_source_file(fragment_shader_path);
    if (!fragment_source_file) {
      ERROR("Failed to find source file at path {}", fragment_shader_path);
      return std::nullopt;
    }

    std::string source_code =
        std::string((std::istreambuf_iterator<char>(fragment_source_file)),
                    std::istreambuf_iterator<char>());

    if (!Shader::create_and_compile_shader(
            GL_FRAGMENT_SHADER, fragment_shader, source_code.c_str(),
            std::string(shader_name) + "::fragment_shader")) {
      return std::nullopt;
    }
  }

  // Create the vertex shader
  {
    std::ifstream vertex_source_file(vertex_shader_path);
    if (!vertex_source_file) {
      ERROR("Failed to find source file at path {}", vertex_shader_path);
      return std::nullopt;
    }

    std::string source_code =
        std::string((std::istreambuf_iterator<char>(vertex_source_file)),
                    std::istreambuf_iterator<char>());

    if (!Shader::create_and_compile_shader(
            GL_VERTEX_SHADER, vertex_shader, source_code.c_str(),
            std::string(shader_name) + "::vertex_shader")) {
      return std::nullopt;
    }
  }

  shader.id = glCreateProgram();
  glAttachShader(shader.id, fragment_shader);
  glAttachShader(shader.id, vertex_shader);
  glLinkProgram(shader.id);
  // check for linking errors
  int success;
  char infoLog[512];
  glGetProgramiv(shader.id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader.id, 512, NULL, infoLog);
    ERROR("Linking stage failed for shader \"{}\":\n{}", shader.name, infoLog);
    return std::nullopt;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return std::optional<Shader>{shader};
}

void Shader::use() {
  DEBUG("Using shader {} with id {}", this->name, this->id);
  glUseProgram(id);
}

void Shader::set_int(const char *name, int var) {
  DEBUG("Setting integer in {}: {}={}", this->name, name, var);

  glUniform1i(glGetUniformLocation(id, name), var);
}

void Shader::set_float(const char *name, float var) {
  DEBUG("Setting float in {}: {}={}", this->name, name, var);

  glUniform1f(glGetUniformLocation(id, name), var);
}

void Shader::set_mat4(const char *name, const glm::mat4x4 &var) {
  DEBUG("Setting mat4 in {}: {}=\n"
        "[\n"
        "  [{:.3}, {:.3}, {:.3}, {:.3}]\n"
        "  [{:.3}, {:.3}, {:.3}, {:.3}]\n"
        "  [{:.3}, {:.3}, {:.3}, {:.3}]\n"
        "  [{:.3}, {:.3}, {:.3}, {:.3}]\n"
        "]",
        this->name, name, var[0][0], var[0][1], var[0][2], var[0][3], var[1][0],
        var[1][1], var[1][2], var[1][3], var[2][0], var[2][1], var[2][2],
        var[2][3], var[3][0], var[3][1], var[3][2], var[3][3]);

  glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE,
                     glm::value_ptr(var));
}

void Shader::set_vec3(const char *name, glm::vec3 v) {
  DEBUG("Setting vec3 in {}: {}=[{:.3}, {:.3}, {:.3}]", this->name, name, v.x,
        v.y, v.z);

  glUniform3f(glGetUniformLocation(id, name), v.x, v.y, v.z);
}

/// The default constructer does not initialize the shader
Shader::Shader(const char *shader_name) : name(shader_name) {}

/// Returns boolean for the success of the compilation
bool Shader::create_and_compile_shader(GLenum type, int &shader_id,
                                       const char *source,
                                       const std::string name

) {
  shader_id = glCreateShader(type);

  GLchar const *src_pointer = source;
  glShaderSource(shader_id, 1, &src_pointer, nullptr);
  glCompileShader(shader_id);

  int success;
  char infoLog[512];
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
    ERROR("Shader compilation failed for shader \"{}\":\n{}", name, infoLog);
    return false;
  }
  return true;
}
