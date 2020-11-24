#pragma once
#include"Header.h"
#ifndef OBJ_H
#include"Obj.h"
#endif // !OBJ_H
#ifndef LIGHT_H
#define LIGHT_H
#endif // !LIGHT_H

constexpr int MAX_LIGHTS{ 8 };

struct LIGHT {
	static GLfloat ambient;
	static glm::vec3 ambientColor;
	static GLuint light_num;
	static std::vector<LIGHT*> lights;
	static GLfloat lights_pos[MAX_LIGHTS * 3];
	static GLfloat lights_col[MAX_LIGHTS * 3];
	static GLint lights_use_spot[MAX_LIGHTS];
	static GLfloat lights_spot_cos[MAX_LIGHTS];
	static GLfloat lights_spot_out_cos[MAX_LIGHTS];
	static GLfloat lights_spot_dir[MAX_LIGHTS * 3];

	Obj obj;
	glm::vec3 spec{ 0.0f,0.0f,0.0f };
	glm::vec3 pos{ 0.0,0.0,0.0 };
	glm::vec3 col{ 1.0,1.0,1.0 };

	bool use_spot{ false };
	glm::vec3 spot_dir;
	GLfloat spot_theta{ 5.0f };
	bool is_on{ false };

	GLfloat spot_cos(int i);

	bool on();
	void off();
	bool on_off();
	void update();
	static void init_light_buffer();
};