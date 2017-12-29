#include <common>

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 v_VertexPosition;
out vec3 v_Normal;
out vec2 v_Texcoord;
out float v_SplatmapMix;

void main()
{
    vec3 worldspacePosition = ObjectToWorld(vertexPosition);

    v_VertexPosition = worldspacePosition;
    v_Normal = LocalNormalToWorld(normal);
    v_Texcoord = texcoord;

    v_SplatmapMix = smoothstep(0., 1., v_VertexPosition.y - 1.);

    gl_Position = ObjectToClipspace(vertexPosition);
}
