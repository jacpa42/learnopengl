#include "mesh.h"
#include "../../glad/include/glad/glad.h"
#include "../../log/log.h"

Mesh::Mesh() {}

void Mesh::use_tex_type(const aiTextureType type) {
  if (type < 32)
    this->used_tex_bm |= (1 << type);
  else
    WARN("Trying to use undefined texture type");
}

std::optional<Mesh> Mesh::create(const char *name, std::vector<Vertex> vertices,
                                 std::vector<unsigned int> indices,
                                 std::vector<TextureId> textures,
                                 unsigned int used_tex_types) {
  if (textures.empty()) {
    WARN("Mesh \"{}\" has no textures!", name);
  }

  if (!vertices.empty() && !indices.empty()) {
    return std::optional<Mesh>(
        Mesh(name, vertices, indices, textures, used_tex_types));
  } else {
    if (vertices.empty())
      ERROR("Mesh \"{}\" has no vertices!", name);
    if (indices.empty())
      ERROR("Mesh \"{}\" has no indices!", name);
    return std::nullopt;
  }
}

Mesh::Mesh(const char *name, std::vector<Vertex> vertices,
           std::vector<unsigned int> indices, std::vector<TextureId> textures,
           unsigned int used_tex_types)
    : name(name), vertices(vertices), indices(indices), texture_ids(textures),
      used_tex_bm(used_tex_types) {
  this->setup();
}

void Mesh::setup() {
  // Generate the buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // Upload vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               &vertices.front(), GL_STATIC_DRAW);
  INFO("Vertex buffer loaded in mesh.name={}", name);

  // Upload index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices.front(), GL_STATIC_DRAW);
  INFO("Index buffer loaded in mesh.name={}", name);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, norm));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_cord));
  glBindVertexArray(0);
}

void Mesh::draw(Shader &shader, const std::vector<Texture> &textures) {
  for (size_t idx = 0; idx < this->texture_ids.size(); idx++) {
    TextureId tid = this->texture_ids[idx];
    std::optional<const Texture *> tex_opt = tid.get_tex_ptr_from(textures);

    if (!tex_opt.has_value()) {
      ERROR("Texture invalidation! {}", tid.idx);
      continue;
    }

    const Texture *tex = *tex_opt;

    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, tex->id);

    WARN("Using texture type {}", tex->material_type_str());
    shader.set_int(tex->material_type_str(), tex->id);
  }

  INFO("");

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
