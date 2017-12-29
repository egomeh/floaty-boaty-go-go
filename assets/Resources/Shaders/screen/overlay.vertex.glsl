layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texcoord;

out vec2 v_Texcoord;

void main()
{
    vec4 position = vec4(vertexPosition, 1.);

    position.z = .5;

    v_Texcoord = texcoord;

    v_Texcoord.y = 1. - v_Texcoord.y;

    gl_Position = position;
}
