in vec3 v_Normal;
in vec2 v_Texcoord;
in vec3 v_ViewDirection;
in vec2 v_Position;

layout(location = 0) out vec4 color;

#include <noise>
#include <common>

uniform sampler2D _MainTex;
uniform vec4 _MainTex_TexelSize;

uniform samplerCube _Skybox;

uniform vec4 _Time;

float WaterHeight(vec2 position)
{
    return sin(position.x * .125 + _Time.x * .35) * .005 * pnoise(vec3(position.x, _Time.x * 0.5, position.y), vec3(33., 33., 33.));
}

vec3 NormalFromHeightmap(float height, float dx, float dy, float stepSize)
{
    float derivativeX = (height - dx) / stepSize;
    float derivativeY = (height - dy) / stepSize;

    return normalize(vec3(derivativeX, .5, derivativeY));
}

void main()
{
    vec3 normalizedViewDirection = normalize(v_ViewDirection);

    const float stepSize = .01;

    float heightSampleCenter = WaterHeight(v_Position);
    float heightSampleDx = WaterHeight(v_Position + vec2(0., stepSize));
    float heightSampleDy = WaterHeight(v_Position + vec2(stepSize, 0.));

    vec3 normal = NormalFromHeightmap(heightSampleCenter, heightSampleDx, heightSampleDy, stepSize);

    float distanceToCamera = length(_CameraPosition .xz- v_Position);

    float normalDistanceFade = clamp(distanceToCamera * .001, 0., 1.);

    normal = normalize(mix(normal, vec3(0., 1., 0.), normalDistanceFade));

    vec3 reflectionRay = reflect(normalizedViewDirection, normal);

    color = texture(_Skybox, reflectionRay);
}
