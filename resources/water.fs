#version 330 core

in vec3 worldFragPos;
in vec2 texCoords;
in vec4 shadowFragPos;
in vec4 projectionFragPos; // for reflection and refraction 可能可以考虑把投影位置转换到纹理坐标的过程直接在 vs 中做完，不知道有没有什么隐患

out vec4 FragColor;

uniform sampler2D shadowMap;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D refractionMap;
uniform sampler2D reflectionMap;

uniform float time;
layout (std140) uniform View {
    vec3 viewPos;
};

#define DISTORTION_SCALE 0.01
#define DISTORTION_SCALE_TIME 0.1
#define DISTORTION_POWER 0.781

layout (std140) uniform Light {
    vec3 lightColor;
    vec3 lightPos;
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
};

vec2 poissonDisk[32] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790),
    vec2(-0.71901624, -0.29906216), vec2(0.84558609, -0.66890725),
    vec2(-0.194184101, -0.82938870), vec2(0.24495938, 0.19387760),
    vec2(-0.81588581, 0.35771432), vec2(-0.71544232, -0.77912464),
    vec2(-0.28277543, 0.17676845), vec2(0.87484398, 0.65648379),
    vec2(0.34323325, -0.87511554), vec2(0.43742981, -0.37373420),
    vec2(-0.16496911, -0.31893023), vec2(0.69197514, 0.09090188),
    vec2(-0.14188840, 0.89706507), vec2(-0.71409955, 0.81437590),
    vec2(0.09984126, 0.68641367), vec2(0.04383161, -0.04100790)
);
const int SAMPLES = 32;
const float blurSize = 32 / 4096.0;
float calcShadow(vec4 FragPos, vec3 normal, vec3 sunDir) {
    float shadow = 0.0;
    vec3 shadowPos = FragPos.xyz / FragPos.w; // 转换到 NDC 坐标系，在透视投影的 w 分量不为 1 时有用
    shadowPos = shadowPos * 0.5 + 0.5; // 转换到 [0, 1] 范围
    float bias = max(0.05 * (1.0 - dot(normal, sunDir)), 0.005);
    float currentDepth = shadowPos.z;
    for (int i = 0; i < SAMPLES; ++i) {
        float pcfDepth = texture(shadowMap, shadowPos.xy + poissonDisk[i] * blurSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= float(SAMPLES);
    if (shadowPos.z > 1.0)
        shadow = 0.0;
    shadow = pow(shadow, 0.781) * (1 - lightAmbient.r);
    return shadow;
}

void main() {
    // 采样 dudvMap 获取扰动值
    vec2 distortion = texture(dudvMap, texCoords).rg * DISTORTION_SCALE; // 只需前两个分量
    distortion = pow(distortion, vec2(DISTORTION_POWER)); // 引入非线性

    vec2 dist_time = vec2(time) * DISTORTION_SCALE_TIME;

    // 采样 normalMap 获取法线
    vec3 normal = texture(normalMap, texCoords + dist_time + distortion).rgb;

    // reflect & refract
    vec2 texCoordRef = vec2(projectionFragPos.x / projectionFragPos.w, projectionFragPos.y / projectionFragPos.w);
    texCoordRef = texCoordRef * 0.5 + 0.5; // 转换到 [0, 1] 范围
    // texCoordRef = clamp(texCoordRef + mod(dist_time, 0.002) - 0.001, 0.0, 1.0); // 扰动
    vec3 refractColor = texture(refractionMap, texCoordRef).rgb;
    vec3 reflectColor = texture(reflectionMap, vec2(1 - texCoordRef.x, texCoordRef.y)).rgb;
    vec3 blueColor = vec3(0.0, 0.25, 1.0);

    vec3 lightDir = normalize(lightPos - worldFragPos);

    // ambient is not affected by shadow or light direction
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff;
    // specular
    vec3 viewDir = normalize(viewPos - worldFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 12);
    vec3 specular = lightSpecular * spec * 0.025; // 把高光调得极低

    float shadow = calcShadow(shadowFragPos, normal, lightDir);

    float fresnel = viewDir.y;// i.e. viewDir.y = dot(vec3(0.0, 1.0, 0.0), viewDir)
    fresnel = clamp(fresnel, 0.2, 0.6);
    vec3 color = fresnel * refractColor + (0.8 - fresnel) * reflectColor + 0.2 * blueColor;
    color = (lightAmbient + (1 - shadow) * (diffuse + specular)) * color; // apply shadow, exclude ambient
    // color = (lightAmbient + (1 - shadow) * smoothstep(0, 1, diffuse + specular)) * color; // 如果用了 smoothstep，反而出不来波纹效果

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(dist_time.x, dist_time.x, dist_time.x, 1.0);
    // FragColor = vec4(shadow, shadow, shadow, 1.0);
}
