//Tolto il warning riguardante l'export delle STL nella dll
#pragma warning(disable:4251)
#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Fbo.h"

#ifdef _WINDOWS
// Export API:
// Specifies i/o linkage (VC++ spec):
#ifdef ENGINE_EXPORTS
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else // Under Linux
#define LIB_API  // Dummy declaration
#endif

#include "Ovr.h"

struct LightSettingsShader {
	int lightPositionLoc;
	int lightAmbientLoc;
	int lightDiffuseLoc;
	int lightSpecularLoc;
};

struct LightSpotSettingShader {
	int direction;
	float cutOff;
	float outerCutOff;

	int lightPositionLoc;
	int lightAmbientLoc;
	int lightDiffuseLoc;
	int lightSpecularLoc;
};

struct ShaderSettings {
	std::shared_ptr<Shader> m_shader;
	int modelview;
	int projection;
	int normalMatLoc;
	int matEmissionLoc;
	int matAmbientLoc;
	int matDiffuseLoc;
	int matSpecularLoc;
	int matShininessLoc;
	LightSettingsShader lightSettings[16];
	LightSpotSettingShader lightSpotSettings[16];
	int num_lights;
	int num_lights_spot;
	int texture;

	std::shared_ptr<Fbo> fbo;
	std::shared_ptr<Shader> passthroughShader;
	int ptProjLoc;
	int ptMvLoc;
	unsigned int boxVertexVbo;
	unsigned int boxTexCoordVbo;
	unsigned int fboTexId;

	std::shared_ptr<OvVR> ovr;

};

class LIB_API Object {
public:
	Object();
	virtual ~Object();

	int id() const;

	void name(std::string name);
	std::string name() const;

	virtual void render(glm::mat4 modelView,ShaderSettings &shader) = 0;

protected:
	const int m_id;
	std::string m_name;
};
