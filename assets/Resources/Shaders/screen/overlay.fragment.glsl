in vec2 v_Texcoord;

layout(location = 0) out vec4 color;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

uniform sampler2D _Image;

void main()
{
    color = texture(_Image, v_Texcoord);
}
