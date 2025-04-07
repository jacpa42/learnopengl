#pragma once

#include "../glad/include/glad/glad.h"
#include "../log/log.h"
#include <assimp/material.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct Texture {
  std::string name = "UNNAMED_TEXTURE";
  aiTextureType type;
  unsigned int id;

  Texture(const char *name, aiTextureType type) : name(name), type(type) {}
  Texture(const std::string name, aiTextureType type)
      : name(name), type(type) {}

  /// Attempts to create a texture with a name and path. If successful it
  /// returns the id of the texture.
  static std::optional<GLint> create(const std::filesystem::path &path);

  /// Returns the name of the uniform which indicates to the renderer that we
  /// should use this texture or not
  constexpr const char *material_use_type_str() const {
    switch (this->type) {
    case aiTextureType_DIFFUSE:
      return "material.use_diffuse";
    case aiTextureType_HEIGHT:
      return "material.use_height";
    case aiTextureType_NORMALS:
      return "material.use_normals";
    case aiTextureType_SPECULAR:
      return "material.use_specular";
    case aiTextureType_EMISSIVE:
      return "material.use_emissive";
    case aiTextureType_AMBIENT:
      return "material.use_ambient";
    case aiTextureType_SHININESS:
      return "material.use_shininess";
    case aiTextureType_OPACITY:
      return "material.use_opacity";
    case aiTextureType_DISPLACEMENT:
      return "material.use_displacement";
    case aiTextureType_LIGHTMAP:
      return "material.use_lightmap";
    case aiTextureType_REFLECTION:
      return "material.use_reflection";
    case aiTextureType_BASE_COLOR:
      return "material.use_base_color";
    case aiTextureType_NORMAL_CAMERA:
      return "material.use_normal_camera";
    case aiTextureType_EMISSION_COLOR:
      return "material.use_emission_color";
    case aiTextureType_METALNESS:
      return "material.use_metalness";
    case aiTextureType_DIFFUSE_ROUGHNESS:
      return "material.use_diffuse_roughness";
    case aiTextureType_AMBIENT_OCCLUSION:
      return "material.use_ambient_occlusion";
    case aiTextureType_SHEEN:
      return "material.use_sheen";
    case aiTextureType_CLEARCOAT:
      return "material.use_clearcoat";
    case aiTextureType_TRANSMISSION:
      return "material.use_transmission";
    case aiTextureType_MAYA_BASE:
      return "material.use_maya_base";
    case aiTextureType_MAYA_SPECULAR:
      return "material.use_maya_specular";
    case aiTextureType_MAYA_SPECULAR_COLOR:
      return "material.use_maya_specular_color";
    case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
      return "material.use_maya_specular_roughness";

    case _aiTextureType_Force32Bit:
      WARN("Attempting to bind texture of type _aiTextureType_Force32Bit. This "
           "is not supported");
      return "";
    case aiTextureType_NONE:
      WARN("Attempting to bind texture of type aiTextureType_NONE. This is not "
           "supported");
      return "";
    case aiTextureType_UNKNOWN:
      WARN("Attempting to bind texture of type aiTextureType_UNKNOWN. This is "
           "not supported");
      return "";
    default:
      return "";
    }
  }

  /// Returns the variable name in the shader for this texture.
  constexpr const char *material_type_str() const {
    switch (this->type) {
    case aiTextureType_DIFFUSE:
      return "material.diffuse";
    case aiTextureType_HEIGHT:
      return "material.height";
    case aiTextureType_NORMALS:
      return "material.normals";
    case aiTextureType_SPECULAR:
      return "material.specular";
    case aiTextureType_EMISSIVE:
      return "material.emissive";
    case aiTextureType_AMBIENT:
      return "material.ambient";
    case aiTextureType_SHININESS:
      return "material.shininess";
    case aiTextureType_OPACITY:
      return "material.opacity";
    case aiTextureType_DISPLACEMENT:
      return "material.displacement";
    case aiTextureType_LIGHTMAP:
      return "material.lightmap";
    case aiTextureType_REFLECTION:
      return "material.reflection";
    case aiTextureType_BASE_COLOR:
      return "material.base_color";
    case aiTextureType_NORMAL_CAMERA:
      return "material.normal_camera";
    case aiTextureType_EMISSION_COLOR:
      return "material.emission_color";
    case aiTextureType_METALNESS:
      return "material.metalness";
    case aiTextureType_DIFFUSE_ROUGHNESS:
      return "material.diffuse_roughness";
    case aiTextureType_AMBIENT_OCCLUSION:
      return "material.ambient_occlusion";
    case aiTextureType_SHEEN:
      return "material.sheen";
    case aiTextureType_CLEARCOAT:
      return "material.clearcoat";
    case aiTextureType_TRANSMISSION:
      return "material.transmission";
    case aiTextureType_MAYA_BASE:
      return "material.maya_base";
    case aiTextureType_MAYA_SPECULAR:
      return "material.maya_specular";
    case aiTextureType_MAYA_SPECULAR_COLOR:
      return "material.maya_specular_color";
    case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
      return "material.maya_specular_roughness";

    case _aiTextureType_Force32Bit:
      WARN("Attempting to bind texture of type _aiTextureType_Force32Bit. This "
           "is not supported");
      return "";
    case aiTextureType_NONE:
      WARN("Attempting to bind texture of type aiTextureType_NONE. This is not "
           "supported");
      return "";
    case aiTextureType_UNKNOWN:
      WARN("Attempting to bind texture of type aiTextureType_UNKNOWN. This is "
           "not supported");
      return "";
    default:
      return "";
    }
  }
};

struct TextureId {
  /// The lower 16 bits are used to reference the texture id and the rest of the
  /// bits are the index in the texture array in the model.
  size_t idx;

  TextureId(unsigned short texture_id, unsigned int index)
      : idx((index << 16) | texture_id) {}

  std::optional<const Texture *>
  get_tex_ptr_from(const std::vector<Texture> &textures) {
    if (index() >= textures.size()) {
      return std::nullopt;
    }

    const Texture *tex = &textures[index()];
    if (tex->id != id()) {
      return std::nullopt;
    }

    return std::optional<const Texture *>(tex);
  }

  constexpr const size_t index() const { return idx >> 16; }
  constexpr const unsigned int id() const { return idx & 0xffff; }
};
