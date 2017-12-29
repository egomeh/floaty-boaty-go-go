in vec3 vp;
in vec2 uvs;

layout(location = 0) out vec4 color;

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

void main()
{
    color = texture(_MainTex, uvs);
}
