in vec2 v_Texcoord;

layout(location = 0) out vec4 color;

#include <fxaa.include>

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

in vec2 v_OffsetNorthWest;
in vec2 v_OffsetNorthEast;
in vec2 v_OffsetSouthWest;
in vec2 v_OffetSouthEast;

// #define SKIP_AA

void main()
{
#ifndef SKIP_AA
    color = fxaa(_MainTex, _MainTex_TexelSize, v_Texcoord, v_OffsetNorthWest, v_OffsetNorthEast, v_OffsetSouthWest, v_OffetSouthEast);
#else
    color = texture(_MainTex, v_Texcoord);
#endif
}


