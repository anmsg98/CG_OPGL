#pragma once
#include"Header.h"
#include"Obj.h"
constexpr int MAX_LIGHTS{ 8 };

struct LIGHT {
	static GLfloat ambient;
	static GLuint light_num;
	static std::vector<LIGHT*> lights;
	static GLfloat lights_pos[MAX_LIGHTS * 3];
	static GLfloat lights_col[MAX_LIGHTS * 3];
	static glm::vec3 ambientColor;
	Obj obj;
	glm::vec3 spec{ 0.0f,0.0f,0.0f };
	glm::vec3 pos{ 0.0,0.0,0.0 };
	glm::vec3 col{ 1.0,1.0,1.0 };

	bool on();
	void off();
	static void init_light_buffer();
};