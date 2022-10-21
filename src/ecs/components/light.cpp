#include "light.hpp"

#include "../../graphics/shader.hpp"
#include "transform.hpp"
#include <iosfwd>
#include <string>

void Light::use(ShaderProgram &shader, Transform &transform, int n) {
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

	shader.tryUniformVec3(prefix + "properties.ambient", ambient);
	shader.tryUniformVec3(prefix + "properties.diffuse", diffuse);
	shader.tryUniformVec3(prefix + "properties.specular", specular);

	if (type != DIRECTIONAL) {
		shader.tryUniformFloat(prefix + "attenuation.linear", linear);
		shader.tryUniformFloat(prefix + "attenuation.quadratic", quadratic);
		shader.tryUniformVec3(prefix + "position", transform.position);
	}

	if (type != POINT) {
		shader.tryUniformVec3(prefix + "direction", transform.rotation);
	}

	if (type == SPOT) {
		shader.tryUniformFloat(prefix + "phi", phi);
		shader.tryUniformFloat(prefix + "gamma", gamma);
	}
}