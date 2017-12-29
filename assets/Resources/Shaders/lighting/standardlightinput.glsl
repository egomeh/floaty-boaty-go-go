
struct LightInfo
{
    vec4 position;
    vec3 intensity;
};

struct MaterialInfo
{
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
  float Shininess;
};

uniform sampler2D _EnvironmentMap;
