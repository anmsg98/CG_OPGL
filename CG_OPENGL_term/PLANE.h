#pragma once
#include"Header.h"
#include"camera.h"
#ifndef OBJ_H
#include"Obj.h"
#endif // !OBJ_H
#ifndef LIGHT_H
#define LIGHT_H
#include"light.h"
#endif // !LIGHT_H

struct PLANE {
	static glm::vec3 grav;
	glm::vec3 roll_dir{1.0,0.0,0.0};

	Obj obj;
	glm::vec3 pos{ 0.0,1000.0,0.0 };
	glm::vec3 coll_size{ 2.0f,1.0f,2.0f };

	COLOR_ color_type{ COLOR_::RED };

	glm::vec3 head{ 0.0,0.0,-1.0 };
	glm::vec3 tail{ 0.0,0.0,1.0 };
	glm::vec3 up{ 0.0,1.0,0.0 };
	GLfloat turn_degree{ 0.0f };

	glm::mat4 viewMat{ df };
	GLfloat view_dist{ 100.0f };

	GLfloat speed{ 10.0f };
	GLfloat maxspeed{ 200.0f };


	LIGHT head_light;
	
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

	void reRoll();
	void go();

	void check_area();

	void set_speed(GLfloat d);

	void setPos();

	void view();

	void view_dist_add(GLfloat dist);

	bool check_coll(glm::vec3 a, glm::vec3 b);
	
	bool check_horizon();

	void HeadLightOnOff();
	void update_head_light();
	void update();
};
extern PLANE plane;