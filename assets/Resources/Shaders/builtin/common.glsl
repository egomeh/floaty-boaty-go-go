
// Common matrices
uniform mat4 _ObjectToClipspace;
uniform mat4 _ObjectToWorld;
uniform mat4 _WorldToCamera;
uniform mat4 _Perspective;
uniform mat3 _NormalMatrix;
uniform vec4 _ProjectionParameters;

// Common parameters
uniform vec4 _ScreenParameters;
uniform vec3 _CameraPosition;

vec4 ObjectToClipspace(vec3 vertexPosition)
{
    return _ObjectToClipspace * vec4(vertexPosition, 1.);
}

vec3 ObjectToWorld(vec3 vertexPosition)
{
    return (_ObjectToWorld * vec4(vertexPosition, 1.)).xyz;
}

vec3 LocalNormalToWorld(vec3 normal)
{
    return _NormalMatrix * normal;
}
