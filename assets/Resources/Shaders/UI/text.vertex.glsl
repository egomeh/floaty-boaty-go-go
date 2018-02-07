layout(location = 0) in vec3 i_VertexPosition;
layout(location = 1) in vec2 i_Texcoord;

out vec3 v_VertexPosition;
out vec2 v_Texcoord;

#include <common>

void main()
{
	vec3 vertexPosition = i_VertexPosition * .1;
    vec4 clipspace = ObjectToClipspace(vertexPosition);
    vec3 worldspace = ObjectToWorld(vertexPosition);

    v_VertexPosition = worldspace;
    v_Texcoord = i_Texcoord;

    gl_Position = clipspace;
}
