
#include <standardlightinput>

float LambertDiffuse(vec3 toLight, vec3 normal)
{
    return max(dot(toLight, normal), 0.);
}

float BlinnPhongSpecular(vec3 toLight, vec3 toEye, vec3 normal, float shininess)
{
    vec3 reflectedLight = reflect(toLight, normal);

    return max(dot(reflectedLight, toEye), 0.);
}

float BlinnPhong(vec3 toLight, vec3 toEye, vec3 normal, float shininess)
{
    float diffuse = LambertDiffuse(toLight, normal);
    float specular = BlinnPhongSpecular(toLight, toEye, normal, shininess);

    return specular + diffuse;
}
