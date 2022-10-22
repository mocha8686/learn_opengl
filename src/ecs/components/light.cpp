#include "light.hpp"

#include "../../graphics/shader.hpp"
#include "transform.hpp"
#include <iosfwd>
#include <string>

void Light::use(std::shared_ptr<ShaderProgram> shader, std::shared_ptr<Transform> transform, glm::mat4 view, int n) {
	std::string prefix;
	switch (type) {
		case DIRECTIONAL:
			prefix = "directionalLights[";
			break;
		case POINT:
			prefix = "pointLights[";
			break;
		case SPOT:
			prefix = "spotLights[";
			break;
	}
	prefix += std::to_string(n) + "].";

	shader->tryUniformVec3(prefix + "properties.ambient", ambient);
	shader->tryUniformVec3(prefix + "properties.diffuse", diffuse);
	shader->tryUniformVec3(prefix + "properties.specular", specular);

	if (type != DIRECTIONAL) {
		shader->tryUniformFloat(prefix + "attenuation.linear", linear);
		shader->tryUniformFloat(prefix + "attenuation.quadratic", quadratic);
		shader->tryUniformVec3(
			prefix + "position",
			glm::vec3(
				view * glm::vec4(
					transform->position.x,
					transform->position.y,
					transform->position.z,
					1.0f
				)
			)
		);
	}

	if (type != POINT) {
		shader->tryUniformVec3(prefix + "direction", transform->rotation);
	}

	if (type == SPOT) {
		shader->tryUniformFloat(prefix + "phi", phi);
		shader->tryUniformFloat(prefix + "gamma", gamma);
	}
}