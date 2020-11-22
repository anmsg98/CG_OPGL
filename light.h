#pragma once
#include"Header.h"
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

	bool on() {
		if (MAX_LIGHTS <= lights.size()) { std::cout << "check"; return false; };
		light_num += 1;
		lights.push_back(this);
		return true;
	}
	void off() {
		std::vector<LIGHT*>::iterator i = std::remove(lights.begin(), lights.end(), this);
		if (i == lights.end())return;
		else {
			light_num -= 1;
			lights.erase(std::remove(lights.begin(), lights.end(), this), lights.end());
		}
	}
	static void init_light_buffer() {
		for (int i = 0; i < light_num; i++) {
			if (lights[i] == nullptr)continue;
			lights_pos[i * 3] = lights[i]->pos.x;
			lights_pos[i * 3 + 1] = lights[i]->pos.y;
			lights_pos[i * 3 + 2] = lights[i]->pos.z;
			lights_col[i * 3] = lights[i]->col.r;
			lights_col[i * 3 + 1] = lights[i]->col.g;
			lights_col[i * 3 + 2] = lights[i]->col.b;
		}
	}
};
GLfloat LIGHT::ambient = 0.7f;
GLuint LIGHT::light_num = 0;
std::vector<LIGHT*> LIGHT::lights;
GLfloat LIGHT::lights_pos[MAX_LIGHTS * 3]{ 0.0f };
GLfloat LIGHT::lights_col[MAX_LIGHTS * 3]{ 0.0f };
glm::vec3 LIGHT::ambientColor{ 1.0,1.0,1.0 };