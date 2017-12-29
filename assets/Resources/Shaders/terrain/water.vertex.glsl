layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec2 v_Texcoord;
out vec3 v_Normal;
out vec3 v_ViewDirection;
out vec2 v_Position;

#include <common>

void main()
{
    float stretch = 5000.;

    vec3 worldspacePosition = vertexPosition * stretch;

    vec4 position = ObjectToClipspace(worldspacePosition);

    v_Texcoord = texcoord * (stretch * .125);

    v_Normal = normal;

    v_ViewDirection = worldspacePosition - _CameraPosition;

    v_Position = vec2(worldspacePosition.x, worldspacePosition.z);

    gl_Position = position;
}
