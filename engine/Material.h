#pragma once
#include "Object.h"
#include "Texture.h"
#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include "Shader.h"

struct MaterialSettings {
	glm::vec3 emission;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float roughness;
};

class LIB_API Material : public Object{
public:
	Material() = default;
	~Material() = default;

	void settings(const MaterialSettings& materialSettings);
	MaterialSettings settings() const;

	void texture(std::shared_ptr<Texture> texture);
	std::shared_ptr<Texture> texture() const;

	//void render(std::shared_ptr<Object> camera);
	void render(glm::mat4 modelView, ShaderSettings &shader);

private:
	MaterialSettings m_settings;
	std::shared_ptr<Texture> m_texture;
};

