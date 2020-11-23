#pragma once
#include"Header.h"
struct CAMERA {
	glm::vec3 EYE{ 0.0f,0.0f,1000.0f };
	glm::vec3 AT{ 0.0f,0.0f,0.0f };
	glm::vec3 UP{ 0.0f,1.0f,0.0f };
	glm::vec3 Dir();
	glm::vec3 Right();
	glm::vec3 Up();
	glm::mat4 view_M();
};
extern CAMERA camera;