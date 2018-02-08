#include <common>
#include <lighting>
#include <toon>

in vec3 v_VertexPosition;
in vec2 v_Texcoord;

layout(location = 0) out vec4 color;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

uniform sampler2D _TextureAtlas;

void main()
{
    color = texture(_TextureAtlas, v_Texcoord);
}
