#pragma once

#include "../glad/include/glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <optional>
#include <string>

class Shader {
public:
  std::string name = "UNNAMED";
  int id;

  Shader();

  /// Attempts to load, compile and link the shader with its fragment and vertex
  /// parts.
  static std::optional<Shader> create(const char *shader_name,
                                      const char *vertex_shader_path,
                                      const char *fragment_shader_path);

  void use();
  void set_int(const char *name, int var);
  void set_float(const char *name, float var);
  void set_mat4(const char *name, const glm::mat4x4 &var);
  void set_vec3(const char *name, glm::vec3 v);

private:
  /// The default constructer does not initialize the shader
  Shader(const char *shader_name);

  /// Returns boolean for the success of the compilation
  static bool create_and_compile_shader(GLenum type, int &shader_id,
                                        const char *source,
                                        const std::string name

  );
};
