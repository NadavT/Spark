#version 450
#extension GL_ARB_separate_shader_objects : enable

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(set = 1, binding = 0) uniform DirectionalLightUBO
{
    DirectionalLight light;
}
directionalLight;

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 10
layout(set = 1, binding = 1) uniform PointLightUBO
{
    PointLight lights[MAX_POINT_LIGHTS];
}
pointLights;

layout(push_constant) uniform PushConsts
{
    int numberOfPointLights;
    bool calcLight;
    vec3 color;
}
pushConsts;

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float innerCutOff;
    float outerCutOff;
};

layout(set = 1, binding = 2) uniform SpotLightUBO
{
    SpotLight light;
}
spotLight;

layout(set = 2, binding = 0) uniform Material
{
    float shininess;
}
material;

#ifdef TEXTURE
layout(set = 3, binding = 0) uniform sampler2D texSampler;
layout(set = 3, binding = 1) uniform sampler2D specularSampler;
#endif

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color, vec3 specularColor)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * specularColor;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, vec3 specularColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * specularColor;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, vec3 specularColor)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * specularColor;
    diffuse *= intensity;
    specular *= intensity;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(-fragPos);

#ifdef COLOR
    vec3 originalColor = pushConsts.color;
    vec3 specularColor = pushConsts.color;
#elif defined(TEXTURE)
    vec3 originalColor = vec3(texture(texSampler, fragTexCoord));
    vec3 specularColor = vec3(texture(specularSampler, fragTexCoord));
#endif

    if (pushConsts.calcLight)
    {
        vec3 finalColor = vec3(0);
        finalColor += CalcDirLight(directionalLight.light, normal, viewDir, originalColor, specularColor);

        for (int i = 0; i < pushConsts.numberOfPointLights; i++)
            finalColor += CalcPointLight(pointLights.lights[i], normal, fragPos, viewDir, originalColor, specularColor);

        finalColor += CalcSpotLight(spotLight.light, normal, fragPos, viewDir, originalColor, specularColor);

        outColor = vec4(finalColor, 1.0);
    }
    else
    {
        outColor = vec4(originalColor, 1.0);
    }
}
