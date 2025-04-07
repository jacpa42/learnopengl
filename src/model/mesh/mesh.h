#pragma once

#include "../../shaders/shader.h"
#include "../../textures/texture.h"
#include <assimp/material.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 tex_cord;

  Vertex() : pos(0), norm(0), tex_cord(0) {}
  Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex_cord)
      : pos(pos), norm(norm), tex_cord(tex_cord) {}
  Vertex(aiVector3D pos, aiVector3D norm, aiVector3D tex_cord)
      : pos(pos.x, pos.y, pos.z), norm(norm.x, norm.y, norm.z),
        tex_cord(tex_cord.x, tex_cord.y) {}
};

struct Mesh {
  std::string name = "UNNAMED_MESH";
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<TextureId> texture_ids;
  /// A bitmap representing whether a specific texture is used or not.
  unsigned int used_tex_bm = 0;

  Mesh();

  void use_tex_type(const aiTextureType type);

  static std::optional<Mesh> create(const char *name,
                                    std::vector<Vertex> vertices,
                                    std::vector<unsigned int> indices,
                                    std::vector<TextureId> textures,
                                    unsigned int used_tex_bm);

  void draw(Shader &shader, const std::vector<Texture> &textures);

private:
  unsigned int VAO, VBO, EBO;

  Mesh(const char *name, std::vector<Vertex> vertices,
       std::vector<unsigned int> indices, std::vector<TextureId> textures,
       unsigned int used_tex_types);

  void setup();
};
