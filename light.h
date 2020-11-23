#pragma once
#include"Header.h"
#ifndef OBJ_H
#include"Obj.h"
#endif // !OBJ_H
#ifndef LIGHT_H
#define LIGHT_H
constexpr int MAX_LIGHTS{ 8 };
#endif // !LIGHT_H


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
	void update();
	static void init_light_buffer();
};