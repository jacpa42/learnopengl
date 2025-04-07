#include "model.h"
#include "../log/log.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <filesystem>
#include <glm/fwd.hpp>
#include <optional>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// We want all meshes from draw_end.. to be non-matching.
//
// Algorithm:
// Maintain two pointers to the start and end of the vector. If first is
// matching, increment its pointer. If last is not matching, decrement its
// pointer. If first is non-matching and last is matching, swap and
// increment/decrement respectively. Repeat while first < right
// clang-format off
void Meshes::update(const std::regex &regex) {
	if (meshes.empty())	return;

  Mesh *l = meshes.data(), *r = l + meshes.size() - 1;
  while (l < r) {
    while (l < r &&  std::regex_match(l->name, regex)) l++;
    while (l < r && !std::regex_match(r->name, regex)) r--;
		std::swap(*l++, *r--);
  }

	draw_end = l - this->meshes.data();
}
// clang-format on

Model::Model(std::filesystem::path model_path, std::vector<Texture> textures,
             std::vector<Mesh> meshes)
    : model_root(model_path.parent_path()), textures(textures), meshes(meshes) {
}

bool Model::process_node(aiNode *node, const aiScene *scene,
                         const char *obj_path) {
  bool success = true;
  for (size_t mesh_idx = 0; mesh_idx < node->mNumMeshes; mesh_idx++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[mesh_idx]];
    auto mesh_opt = process_mesh(mesh, scene, obj_path);
    if (mesh_opt.has_value()) {
      meshes.push_back(*mesh_opt);
    } else {
      ERROR("Failed to process mesh: {}", mesh->mName.data);
      success = false;
    }
  }

  for (size_t child_idx = 0; child_idx < node->mNumChildren; child_idx++) {
    if (!process_node(node->mChildren[child_idx], scene, obj_path)) {
      ERROR("Failed to process node: {}. Exiting",
            node->mChildren[child_idx]->mName.data);
      success = false;
    }
  }
  return success;
}

void Model::load_mesh_textures(Mesh &mesh, aiMaterial *material,
                               aiTextureType type) {
  for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
    // Get the name of the texture
    std::string filename_with_suffix;
    {
      aiString str;
      material->GetTexture(type, i, &str);
      const char *rend = str.data - 1;

      for (char *c = str.data + str.length - 1; c > rend; c--) {
        // As the path might be windows or unix, we need to check for '\' and
        // '/'.
        if (*c == '/' || *c == '\\') {
          filename_with_suffix = std::string(++c);
          break;
        }
      }
      if (filename_with_suffix.empty()) {
        filename_with_suffix = std::string(str.data);
      }
    }
    std::filesystem::path texture_path =
        (model_root / "textures" / filename_with_suffix);

    // Get the name of the texture
    std::string filename_no_suffix =
        std::filesystem::path(filename_with_suffix).stem();

    INFO("Attempting to load texture at path {}", texture_path.c_str());

    bool found = false;
    for (size_t tex_idx = 0; tex_idx < this->textures.size(); tex_idx++) {
      if (this->textures[tex_idx].name == filename_no_suffix) {
        found = true;
        const Texture &tex = this->textures[tex_idx];
        WARN("Adding texture {} for mesh ", tex.name, mesh.name);
        mesh.texture_ids.emplace_back(tex.id, tex_idx);
        mesh.use_tex_type(type);
        break;
      }
    }

    if (!found) {
      size_t tex_idx = this->textures.size();
      Texture &tex = this->textures.emplace_back(filename_no_suffix, type);
      auto id_opt = Texture::create(texture_path);
      if (id_opt.has_value()) {
        tex.id = *id_opt;
        WARN("Adding texture {} for mesh ", tex.name, mesh.name);
        mesh.texture_ids.emplace_back(tex.id, tex_idx);
        mesh.use_tex_type(type);
      } else {
        ERROR("Failed to create texture with path {}", texture_path.c_str());
      }
    }
  }
}

std::optional<Mesh> Model::process_mesh(aiMesh *ai_mesh, const aiScene *scene,
                                        const char *obj_path) {
  Mesh mesh;

  // Reserve and append all vertices
  mesh.vertices.reserve(ai_mesh->mNumVertices);
  if (ai_mesh->mTextureCoords[0] == nullptr) {
    WARN("The mesh->mTextureCoords[0] is null");
    for (size_t idx = 0; idx < ai_mesh->mNumVertices; idx++) {
      mesh.vertices.emplace_back(ai_mesh->mVertices[idx],
                                 ai_mesh->mNormals[idx], aiVector3D(0));
    }
  } else {
    for (size_t idx = 0; idx < ai_mesh->mNumVertices; idx++) {
      mesh.vertices.emplace_back(ai_mesh->mVertices[idx],
                                 ai_mesh->mNormals[idx],
                                 ai_mesh->mTextureCoords[0][idx]);
    }
  }

  // Append all indices
  for (size_t face_idx = 0; face_idx < ai_mesh->mNumFaces; face_idx++) {
    for (size_t idx = 0; idx < ai_mesh->mFaces[face_idx].mNumIndices; idx++) {
      mesh.indices.push_back(ai_mesh->mFaces[face_idx].mIndices[idx]);
    }
  }

  // Append all textures
  aiMaterial *material = scene->mMaterials[ai_mesh->mMaterialIndex];
  aiTextureType texture_types[] = {
      aiTextureType_DIFFUSE, aiTextureType_HEIGHT, aiTextureType_NORMALS,
      aiTextureType_SPECULAR, aiTextureType_EMISSIVE};

  for (const auto type : texture_types) {
    load_mesh_textures(mesh, material, type);
  }

  return Mesh::create(ai_mesh->mName.data, mesh.vertices, mesh.indices,
                      mesh.texture_ids, mesh.used_tex_bm);
}

std::optional<Model> Model::load(const char *src_path) {
  if (!std::filesystem::exists(src_path)) {
    ERROR("Model source path {} doesn\'t exist!", src_path);
    return std::nullopt;
  }

  Model model(src_path, {}, {});
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      src_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                    aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (scene == nullptr) {
    ERROR("ASSIMP error info:\n{}", importer.GetErrorString());
    return std::nullopt;
  } else {
    INFO("Loaded model {}", src_path);
  }

  if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) > 0) {
    ERROR("Failed to load model at path {}. AI_SCENE_FLAGS_INCOMPLETE set true",
          src_path);
    ERROR("ASSIMP error info:\n{}", importer.GetErrorString());
    return std::nullopt;
  }

  bool success = model.process_node(scene->mRootNode, scene, src_path);
  if (!success) {
    ERROR("Failed to process root node in Model::load({})", src_path);
    return std::nullopt;
  }

  return std::optional<Model>(model);
}
