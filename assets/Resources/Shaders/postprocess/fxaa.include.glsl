#define FXAA_REDUCE_MIN (1.0/ 128.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX 8.0
#define LUMA_THREASHOLD 0.1

// Modified version of https://stackoverflow.com/questions/12105330/how-does-this-simple-fxaa-work
vec4 fxaa(sampler2D tex, vec4 texelSize, vec2 texcoord, vec2 offsetNorthWest, vec2 offsetNorthEast, vec2 offsetSouthWest, vec2 offsetSouthEast)
{
    vec3 rgbCenter = texture(tex, texcoord).rgb;
    vec3 rgbNorthWest = texture(tex, offsetNorthWest).rgb;
    vec3 rgbNorthEast = texture(tex, offsetNorthEast).rgb;
    vec3 rgbSouthWest = texture(tex, offsetSouthWest).rgb;
    vec3 rgbSouthEast = texture(tex, offsetNorthEast).rgb;

    vec3 luma = vec3(.299, .587, .114);

    float lumaCenter = dot(rgbCenter, luma);
    float lumaNorthWest = dot(rgbNorthWest, luma);
    float lumaNorthEast = dot(rgbNorthEast, luma);
    float lumaSouthWest = dot(rgbSouthWest, luma);
    float lumaSouthEast = dot(rgbSouthEast, luma);

    float lumaMin = min(lumaCenter, min(min(lumaNorthWest, lumaNorthEast), min(lumaSouthWest, lumaSouthEast)));
    float lumaMax = max(lumaCenter, max(max(lumaNorthWest, lumaNorthEast), max(lumaSouthWest, lumaSouthEast)));

    if (lumaMax - lumaMin < lumaMax * LUMA_THREASHOLD)
    {
        return vec4(rgbCenter, 1.);
    }

    vec2 direction;
    direction.x = -((lumaNorthWest + lumaNorthEast) - (lumaSouthWest + lumaSouthEast));
    direction.y =  ((lumaNorthWest + lumaSouthWest) - (lumaNorthEast + lumaSouthEast));

    float lumaSum = lumaNorthWest + lumaNorthEast + lumaSouthWest + lumaSouthEast;
    float directionReduce = max(lumaSum * .25 * FXAA_REDUCE_MUL, FXAA_REDUCE_MIN);

    float reciprocalDirectionMin = 1. / (min(abs(direction.x), abs(direction.y)) + directionReduce);

    direction = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), direction * reciprocalDirectionMin)) * texelSize.xy;

    vec3 colorSampleNegative = texture(tex, texcoord + direction * (1. / 3. - .5)).rgb;
    vec3 colorSamplePositive = texture(tex, texcoord + direction * (2. / 3. - .5)).rgb;

    vec3 colorTwoTab = (colorSampleNegative + colorSamplePositive) * .5;

    vec3 colorSampleNegativeFar = texture(tex, texcoord + direction * (0. / 3. - .5)).rgb;
    vec3 colorSamplePositiveFar = texture(tex, texcoord + direction * (3. / 3. - .5)).rgb;

    vec3 colorFourTab = (colorSampleNegativeFar + colorSamplePositiveFar) * .25 + colorTwoTab * .5;

    float lumaFourTab = dot(colorFourTab, luma);

    float tabChoice = step(lumaFourTab, lumaMin) * (1. - step(lumaMax, lumaFourTab));

    vec3 color = mix(colorTwoTab, colorFourTab, tabChoice);

    return vec4(color, 1.);
}
