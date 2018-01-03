layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texcoord;

out vec2 v_Texcoord;

out vec2 v_OffsetNorthWest;
out vec2 v_OffsetNorthEast;
out vec2 v_OffsetSouthWest;
out vec2 v_OffsetSouthEast;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

void main()
{
    v_Texcoord = texcoord;

    v_OffsetNorthWest = texcoord + vec2(-1., -1.) * _MainTex_TexelSize.xy;
    v_OffsetNorthEast = texcoord + vec2( 1., -1.) * _MainTex_TexelSize.xy;
    v_OffsetSouthWest = texcoord + vec2(-1.,  1.) * _MainTex_TexelSize.xy;
    v_OffsetSouthEast = texcoord + vec2( 1.,  1.) * _MainTex_TexelSize.xy;

    gl_Position = vec4(vertexPosition, 1.);
}
