#pragma once

#include <glm/glm.hpp>
#include <cmath>

class ShaderProgram;
struct Transform;

enum LightType {
	DIRECTIONAL,
	POINT,
	SPOT,
};

struct Light {
	LightType type;

	glm::vec3 ambient = glm::vec3(0.05f);
	glm::vec3 diffuse = glm::vec3(0.8f);
	glm::vec3 specular = glm::vec3(1.0f);

	float linear = .09f;
	float quadratic = .032f;

	float phi = cos(glm::radians(12.5f));
	float gamma = cos(glm::radians(15.0f));

	Light(LightType type) : type(type) {};
	void use(ShaderProgram &shader, Transform &transform, int n);
};