layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 v_VertexPosition;
out vec3 v_Normal;
out vec2 v_Texcoord;

#include <common>

void main()
{
    vec4 clipspace = ObjectToClipspace(vertexPosition);
    vec3 worldspace = ObjectToWorld(vertexPosition);

    v_VertexPosition = worldspace;
    v_Normal = LocalNormalToWorld(normal);
    v_Texcoord = texcoord;

    gl_Position = clipspace;
}
