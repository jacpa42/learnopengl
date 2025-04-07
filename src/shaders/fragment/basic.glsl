#version 460 core
out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D height;
  sampler2D normals;
  sampler2D specular;
  sampler2D emissive;

  bool use_diffuse;
  bool use_height;
  bool use_normals;
  bool use_specular;
  bool use_emissive;
};

struct Light {
  vec3 pos;
  vec3 col;
};

in vec2 tex_coords;
in vec3 fragment_positon;
in vec3 normal;

uniform vec3 camera_position;
uniform Material material;
uniform Light light;

void main() {
  // Height map adjustment (simple parallax effect)
  float height = texture(material.height, tex_coords).r;
  vec2 parallaxTexCoords = tex_coords + (normal.xy * (height * 0.1));

  // Light direction
  vec3 lightDir = normalize(light.pos - fragment_positon);

  // Diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse =
      diff * light.col * texture(material.diffuse, parallaxTexCoords).rgb;

  // Specular reflection
  vec3 viewDir = normalize(camera_position - fragment_positon);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular =
      spec * light.col * texture(material.specular, parallaxTexCoords).rgb;

  // Combine results
  vec3 result = diffuse + specular;
  FragColor = vec4(result, 1.0);
}
