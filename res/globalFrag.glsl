#version 330 core

const int MAX_LIGHTS = 32;

struct Material {
	sampler2D texDiffuse0;
	sampler2D texSpecular0;
	float shininess;
};

struct LightProperties {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct AttenuationProperties {
	float linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	LightProperties properties;
};

struct PointLight {
	vec3 position;
	AttenuationProperties attenuation;
	LightProperties properties;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float phi;
	float gamma;
	AttenuationProperties attenuation;
	LightProperties properties;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;
out vec4 fragColor;

uniform Material material;

uniform int nDirectionalLights;
uniform DirectionalLight directionalLights[MAX_LIGHTS];

uniform int nPointLights;
uniform PointLight pointLights[MAX_LIGHTS];

uniform int nSpotLights;
uniform SpotLight spotLights[MAX_LIGHTS];

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
	vec3 diffuseMapValue = texture(material.texDiffuse0, texCoords).rgb;

	float diffuseValue = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.properties.ambient * diffuseMapValue;
	vec3 diffuse = light.properties.diffuse * diffuseMapValue * diffuseValue;
	vec3 specular = light.properties.specular * specularValue * texture(material.texSpecular0, texCoords).rgb;

	return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 diffuseMapValue = texture(material.texDiffuse0, texCoords).rgb;

	float diffuseValue = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float dist = distance(light.position, fragPos);
	float attenuation = 1.0 / (1.0 + light.attenuation.linear * dist + light.attenuation.quadratic * dist * dist);

	vec3 ambient = light.properties.ambient * diffuseMapValue;
	vec3 diffuse = light.properties.diffuse * diffuseMapValue * diffuseValue;
	vec3 specular = light.properties.specular * texture(material.texSpecular0, texCoords).rgb * specularValue;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 diffuseMapValue = texture(material.texDiffuse0, texCoords).rgb;

	float diffuseValue = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float dist = distance(light.position, fragPos);
	float attenuation = 1.0 / (1.0 + light.attenuation.linear * dist + light.attenuation.quadratic * dist * dist);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.phi - light.gamma;
	float intensity = clamp((theta - light.gamma) / epsilon, 0.0, 1.0);

	vec3 ambient = light.properties.ambient * diffuseMapValue;
	vec3 diffuse = light.properties.diffuse * diffuseMapValue * diffuseValue;
	vec3 specular = light.properties.specular * texture(material.texSpecular0, texCoords).rgb * specularValue;

	return (ambient + diffuse + specular) * attenuation * intensity;
}

void main() {
	vec3 normal = normalize(normal);

	vec3 result = vec3(0.0);

	for (int i = 0; i < nDirectionalLights; i++)
		result += calculateDirectionalLight(directionalLights[i], normal, normalize(-fragPos));

	for (int i = 0; i < nPointLights; i++)
		result += calculatePointLight(pointLights[i], normal, fragPos);

	for (int i = 0; i < nSpotLights; i++)
		result += calculateSpotLight(spotLights[i], normal, fragPos);

	fragColor = vec4(result, 1.0);
}