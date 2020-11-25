#pragma once
#include"Header.h"


struct SCREEN {
	GLfloat fovy{ 60.0f }; //glm::Radians(fovy)
	GLsizei width{ 800 };	//W/H
	GLsizei height{ 600 };	//W/H
	GLfloat n{ 0.1f };
	GLfloat f{ 200000.0f };
	glm::vec3 size_of_world{ 100.0f,100.0f,100.0f };//[-a:a]
	GLfloat aspect();
	glm::mat4 proj_M();
};
extern SCREEN screen;