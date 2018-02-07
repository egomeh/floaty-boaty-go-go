#include <common>
#include <lighting>
#include <toon>

in vec3 v_VertexPosition;
in vec2 v_Texcoord;

layout(location = 0) out vec4 color;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

uniform sampler2D _Albedo;

void main()
{
    vec3 toLight = normalize(vec3(1., 1., 0.));
    vec3 toEye = normalize(_CameraPosition - v_VertexPosition);

    color = texture(_Albedo, v_Texcoord);
}
