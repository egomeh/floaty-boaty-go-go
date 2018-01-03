layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texcoord;

out vec2 v_Texcoord;

void main()
{
    v_Texcoord = texcoord;
    gl_Position = vec4(vertexPosition, 1.);
}
