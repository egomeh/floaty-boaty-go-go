in vec3 v_VertexPosition;
in vec2 v_Texcoord;

layout(location = 0) out vec4 color;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

void main()
{
    color = texture(_MainTex, v_Texcoord);
}
