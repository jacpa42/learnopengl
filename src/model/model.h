#pragma once

#include "../shaders/shader.h"
#include "../textures/texture.h"
#include "mesh/mesh.h"
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <filesystem>
#include <optional>
#include <regex>
#include <vector>

/// A nice way of keeping track of which meshes to draw
struct Meshes {
  Meshes(std::vector<Mesh> meshes) : meshes(meshes), draw_end(meshes.size()) {}

  /// Updates the internal draw list to include or exclude certain meshes which
  /// match the regex.
  ///
  /// Any meshes whose name matches the regex are set to drawing and the others
  /// are set to not drawing
  void update(const std::regex &regex);

  /// inserts the mesh and sets it to be drawable
  void push_back(const Mesh &mesh) {
    size_t inserted_at = meshes.size();
    meshes.push_back(mesh);
    if (draw_end < inserted_at)
      std::swap(meshes[draw_end], meshes.back());
    draw_end++;
  }

  void draw(Shader &shader, const std::vector<Texture> &textures) {
    for (size_t i = 0; i < draw_end; i++)
      meshes[i].draw(shader, textures);
  }

private:
  std::vector<Mesh> meshes;
  /// Represents the last item in the mesh which should be drawn
  size_t draw_end;
};

struct Model {
  /// The name of the model. Mostly used for debugging purposes
  const char *name;
  Meshes meshes;

  Model(const char *name) : name(name), meshes({}) {}

  /// Loads the model given a directory `dir`
  static std::optional<Model> load(const char *dir);

  /// Checks to see if the directory path is a valid format for the parser
  ///
  /// We expect the following structure
  /// root_dir / model.`{format}`
  /// 				 / textures / *.`{texture_format}`
  static bool directory_path_valid(const std::filesystem::path &path);

  /// Loads the source file from the root directory. Asssumes its the only file
  /// without the exstension `png` or `jpeg`
  std::optional<std::filesystem::path> get_source();

  /// Draws the mesh using the provided shader
  void draw(Shader &shader) { meshes.draw(shader, textures); }

private:
  std::filesystem::path model_root;
  std::vector<Texture> textures;

  Model(std::filesystem::path model_path, std::vector<Texture> textures,
        std::vector<Mesh> meshes);
  /// Process a scene node and returns whether it was a success.
  bool process_node(aiNode *mesh, const aiScene *scene, const char *obj_path);

  /// Loads the textures for a specific mesh. Also checks that the texture has
  /// not already been loaded via the filename.
  void load_mesh_textures(Mesh &mesh, aiMaterial *material, aiTextureType type);

  /// Attempts to construct a mesh from the aiMesh
  std::optional<Mesh> process_mesh(aiMesh *mesh, const aiScene *scene,
                                   const char *obj_path);
};
