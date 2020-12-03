#include"light.h"

GLfloat LIGHT::ambient = 0.2f;
GLuint LIGHT::light_num = 0;
std::vector<LIGHT*> LIGHT::lights;
GLfloat LIGHT::lights_pos[MAX_LIGHTS * 3]{ 0.0f };
GLfloat LIGHT::lights_col[MAX_LIGHTS * 3]{ 0.0f };
GLint LIGHT::lights_use_spot[MAX_LIGHTS]{ 0 };
GLfloat LIGHT::lights_spot_cos[MAX_LIGHTS]{ 2.0f };
GLfloat LIGHT::lights_spot_out_cos[MAX_LIGHTS]{ 2.0f };
GLfloat LIGHT::lights_spot_dir[MAX_LIGHTS * 3]{ 0.0f };
glm::vec3 LIGHT::ambientColor{ 1.0,1.0,1.0 };


GLfloat LIGHT::spot_cos(int i) { float a = 1.0f + i * 1.5f; if (this->use_spot)return glm::cos(glm::radians(this->spot_theta*a)); else return 2.0f;/*abs()<=1.0*/ }

bool LIGHT::on() {
	if (MAX_LIGHTS <= lights.size()) { std::cout << "light_max_num_check"; return false; };
	if (find(lights.begin(), lights.end(), this) == lights.end()) {
		light_num += 1;
		
		lights.push_back(this);
		this->is_on = true;
		return true;
	}
}
void LIGHT::off() {
	std::vector<LIGHT*>::iterator i = std::remove(lights.begin(), lights.end(), this);
	if (i == lights.end())return;
	else {
		light_num -= 1;
		lights.erase(i, lights.end());
		this->is_on = false;
	}
}
bool LIGHT::on_off() {
	if (this->is_on) { this->off(); return false; }
	else { this->on(); return true; }
}


void LIGHT::update() {
	this->obj.M.at(0) = glm::translate(df, this->pos);
}

void LIGHT::init_light_buffer() {
	for (int i = 0; i < light_num; i++) {
		if (lights[i] == nullptr)continue;
		lights_pos[i * 3] = lights[i]->pos.x;
		lights_pos[i * 3 + 1] = lights[i]->pos.y;
		lights_pos[i * 3 + 2] = lights[i]->pos.z;
		lights_col[i * 3] = lights[i]->col.r;
		lights_col[i * 3 + 1] = lights[i]->col.g;
		lights_col[i * 3 + 2] = lights[i]->col.b;
		lights_use_spot[i] = lights[i]->use_spot;
		lights_spot_cos[i] = lights[i]->spot_cos(0);
		lights_spot_out_cos[i] = lights[i]->spot_cos(1);
		lights_spot_dir[i * 3] = lights[i]->spot_dir.x;
		lights_spot_dir[i * 3 + 1] = lights[i]->spot_dir.y;
		lights_spot_dir[i * 3 + 2] = lights[i]->spot_dir.z;
	}
}