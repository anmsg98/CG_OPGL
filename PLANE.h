#pragma once
#include"Header.h"
#include"camera.h"
#ifndef OBJ_H
#include"Obj.h"
#endif // !OBJ_H
struct PLANE {
	Obj obj;
	glm::vec3 pos{ 0.0,-100.0,0.0 };
	glm::vec3 head{ 0.0,0.0,-1.0 };
	glm::vec3 tail{ 0.0,0.0,1.0 };
	glm::vec3 up{ 0.0,1.0,0.0 };

	size_t eye_mode = 0;
	GLfloat speed{ 0.0f };
	GLfloat maxspeed{ 20.0f };
	void init();


	void default_color();
	void Stealth(bool on);
	void update_coor(glm::mat4& m);

	glm::vec3 nDir();
	glm::vec3 Right();
	glm::vec3 Up();

	glm::mat4 yaw_(GLfloat degree);
	glm::mat4 pitch_(GLfloat degree);
	glm::mat4 roll_(GLfloat degree);

	void Yaw(GLfloat degree);
	void Pitch(GLfloat degree);
	void Roll(GLfloat degree);

	void go();

	void check_area();

	void set_speed(GLfloat d);

	void setPos();

	void view();

	void update(bool s);
};
extern PLANE plane;