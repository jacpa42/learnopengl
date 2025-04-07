#include "texture.h"
#include "../glad/include/glad/glad.h"
#include "../log/log.h"
#include <cassert>
#include <optional>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/// Attempts to create a texture with a name and path. If successful it
/// returns the id of the texture.
std::optional<GLint> Texture::create(const std::filesystem::path &path) {
  if (!std::filesystem::is_regular_file(path)) {
    ERROR("Failed to load texture at path \"{}\". This file does not exist",
          path.c_str());
    return std::nullopt;
  }
  unsigned int id;
  glGenTextures(1, &id);
  INFO("Generated texture \"{}\" with id={}", path.c_str(), id);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
  if (!data) {
    stbi_image_free(data);
    ERROR("stbi failed to load texture from \"{}\"", path.c_str());
    return std::nullopt;
  }

  GLenum format;
  if (nrComponents == 1) {
    INFO("\"{}\".format = GL_RED", path.c_str());
    format = GL_RED;
  } else if (nrComponents == 3) {
    INFO("\"{}\".format = GL_RGB", path.c_str());
    format = GL_RGB;
  } else if (nrComponents == 4) {
    INFO("\"{}\".format = GL_RGBA", path.c_str());
    format = GL_RGBA;
  } else {
    ERROR("Failed to load texture at path \"{}\"", path.c_str());
    return std::nullopt;
  }

  assert(id >= 1);
  glActiveTexture(GL_TEXTURE0 + id - 1);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return std::optional<GLint>(id);
}
