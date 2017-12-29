#include <common>
#include <lighting>
#include <toon>

in vec3 v_VertexPosition;
in vec3 v_Normal;
in vec2 v_Texcoord;
in float v_SplatmapMix;

layout(location = 0) out vec4 color;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

uniform sampler2D _Grass;
uniform sampler2D _Sand;

void main()
{
    vec3 grassColor = texture(_Grass, v_Texcoord).rgb;
    vec3 sandColor = texture(_Sand, v_Texcoord).rgb;

    vec3 toLight = normalize(vec3(1., 1., 0.));
    vec3 toEye = normalize(_CameraPosition - v_VertexPosition);
    vec3 normal = normalize(v_Normal);

    vec3 splatAlbedo = mix(sandColor, grassColor, v_SplatmapMix);

    float lightContribution = LambertDiffuse(toLight, normal);

    // Ambient contribution
    lightContribution += .25;

    color = vec4(splatAlbedo, 1.) * lightContribution;
}
