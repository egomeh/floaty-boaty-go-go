layout(location = 0) in vec3 vertexPosition;

#include <common>

out vec3 v_LookDirection;

void main()
{
    v_LookDirection = normalize(vertexPosition);

    vec4 clipspace = ObjectToClipspace(vertexPosition);

    // Put the fragment furthest behind in the depth buffer
    // So that all fragments fail the depth test
    gl_Position = clipspace.xyww;
}
