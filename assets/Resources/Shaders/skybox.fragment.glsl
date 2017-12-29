layout(location = 0) out vec4 color;

uniform samplerCube _Skybox;

in vec3 v_LookDirection;

void main()
{
    color = texture(_Skybox, v_LookDirection);
}
