layout(location = 0) in vec3 vp;
layout(location = 1) in vec2 uv;

out vec2 uvs;

void main()
{
    uvs = uv;
    gl_Position = vec4(vp, 1.);
}
