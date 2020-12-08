#include"PLANE.h"
#include"texture_sunkue.h"

PLANE plane;
#define ROOT2 1.41421

//중력장
glm::vec3 PLANE::grav{ 0.0,-1.0,0.0 };


void PLANE::init() {
	// 0 Trans	1 Yaw	2 Pitch	3 Roll	4 Size
	LoadObj("airplane.obj", this->obj, "8/8/8");
	this->obj.M.resize(3, df);
	this->obj.M.at(2) = glm::scale(df, glm::vec3(0.25f));
	this->obj.M.at(1) = glm::rotate(df, glm::radians(180.0f), { 0.0,1.0,0.0 });
	this->setPos();
	LoadTexture(this->obj, "red.jpg", 512, 512, 3);
	this->head_light.col = { 1.0,1.0,1.0 };
	this->head_light.use_spot = true;
}


void PLANE::default_color() {
	this->obj.Set_Color({ 0.5,0.8,0.2,1.0 });
}
void PLANE::Stealth(bool on) {
	if (on)obj.Set_Color({ 0.5,0.0,1.0,0.5 });
	else this->default_color();
}

void PLANE::update_coor(glm::mat4& m) {
	head = glm::normalize(m * glm::vec4(head, 1.0f));
	tail = glm::normalize(m * glm::vec4(tail, 1.0f));
	up = glm::normalize(m * glm::vec4(up, 1.0f));
}

glm::vec3 PLANE::nDir() { return glm::normalize(this->tail - this->head); }
glm::vec3 PLANE::Right() { return glm::normalize(glm::cross(this->up, this->nDir())); }
glm::vec3 PLANE::Up() { return glm::normalize(glm::cross(this->nDir(), this->Right())); }

glm::mat4 PLANE::yaw_(GLfloat degree) { return glm::rotate(df, glm::radians(degree), Up()); }
glm::mat4 PLANE::pitch_(GLfloat degree) { return glm::rotate(df, glm::radians(degree), Right()); }
glm::mat4 PLANE::roll_(GLfloat degree) { return glm::rotate(df, glm::radians(degree), nDir()); }


void PLANE::Yaw(GLfloat degree) {
	glm::mat4 m = yaw_(degree);
	obj.M.at(1) = m * obj.M.at(1);
	update_coor(m);
}
void PLANE::Pitch(GLfloat degree) {
	glm::mat4 m = pitch_(degree);
	obj.M.at(1) = m * obj.M.at(1);
	update_coor(m);
}
void PLANE::Roll(GLfloat degree) {
	glm::mat4 m = roll_(degree);
	obj.M.at(1) = m * obj.M.at(1);
	update_coor(m);
}

void PLANE::reRoll() {
	GLfloat cos = glm::dot(glm::normalize(PLANE::grav), glm::normalize(this->Right()));
	GLfloat degree = glm::degrees(glm::acos(cos)) - 90.0f;

	// ABCDEF reroll speed
	constexpr float c{ 30.0f };

	this->Roll(-degree / c);
	if (abs(degree) < 0.01f) {
		this->Roll(-degree);
	}
}

void PLANE::go() {
	// ABCDEF 4,6 speed
	constexpr GLfloat rollspeed{ -2.0f };
	GLfloat cos = glm::dot(glm::normalize(PLANE::grav), glm::normalize(this->Right()));
	GLfloat degree = glm::degrees(glm::acos(cos)) - 90.0f;
	GLfloat t = glm::sin(glm::radians(degree)) * rollspeed * speed;
	glm::vec3 D{ plane.Right() };

	D.y = 0.0f;
	D = glm::normalize(D);

	glm::mat4 m = glm::translate(df, nDir() * -speed) * glm::translate(df, D * t) * glm::translate(df, PLANE::grav);
	//go
	this->pos = m * glm::vec4{ this->pos,1.0f };
	check_area();
}

void PLANE::check_area() {
	/*바닥 체크*/
	if (this->pos.y < ground_floor + 15.0f) {
		//this->obj.Set_Color({ 1.0,0.2,0.2,1.0 });
		this->pos.y = ground_floor + 15.0f;
	}
}

void PLANE::set_speed(GLfloat d) {
	speed += d;
	if (speed < 0.0f)speed = 0.0f;
	if (maxspeed < speed)speed = maxspeed;
}

void PLANE::setPos() {
	this->obj.M.at(0) = glm::translate(df, pos);
}

void PLANE::view() {
	camera.AT = glm::translate(df, nDir() * -15.0f) * glm::vec4(this->pos, 1.0f);
	camera.EYE = glm::translate(df, this->pos) * this->viewMat * glm::translate(df, this->nDir() *view_dist) * glm::vec4(1.0f);
	if (camera.EYE.y < ground_floor + 20.0f)camera.EYE.y = ground_floor + 20.0f;
}

void PLANE::view_dist_add(GLfloat dist) {
	this->view_dist += dist;
	if (view_dist < 15.0f)view_dist = 15.0f;
	if (400.0f < view_dist)view_dist = 400.0f;
}

void PLANE::HeadLightOnOff() { this->head_light.on_off(); };


void PLANE::update_head_light(){
	this->head_light.pos = glm::translate(df, this->nDir() * -10.0f) * glm::vec4(this->pos, 1.0);
	this->head_light.spot_dir = this->nDir() * -100.0f;
}


bool PLANE::check_horizon() {
	bool check{ false };
	GLfloat x{ this->pos.x };
	GLfloat z{ this->pos.z };
	constexpr GLfloat size{ groundsize * 2.0f };
	if (x < -groundsize) {
		this->pos = glm::translate(df, { size, 0.0, 0.0 }) * glm::vec4(this->pos, 1.0f);
		check = true;
	}
	else if (groundsize < x) {
		this->pos = glm::translate(df, { -size, 0.0, 0.0 }) * glm::vec4(this->pos, 1.0f);
		check = true;
	}
	else if (z < -groundsize) {
		this->pos = glm::translate(df, { 0.0, 0.0, size }) * glm::vec4(this->pos, 1.0f);
		check = true;
	}
	else if (groundsize < z) {
		this->pos = glm::translate(df, { 0.0, 0.0, -size }) * glm::vec4(this->pos, 1.0f);
		check = true;
	}
	return check;
}

bool PLANE::check_coll(glm::vec3 a, glm::vec3 b) {
	// x, y, z 높이 전좌하 후우상		(a,b)(x,y,z)[<,<,<][7,0]
	if (b.x < a.x)std::swap(a.x, b.x);
	if (b.y < a.y)std::swap(a.y, b.y);
	if (b.z < a.z)std::swap(a.z, b.z);
	glm::vec3 p = this->pos, coll = this->coll_size;
	glm::mat4 M = this->obj.world_M();
	glm::vec3 nn{ (speed / 20.0f * -nDir()) };
	glm::vec3 pos[9]{ 
		p,
		p + glm::vec3{M * glm::vec4{  coll.x, coll.y, coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{ -coll.x, coll.y, coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{  coll.x,-coll.y, coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{ -coll.x,-coll.y, coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{  coll.x, coll.y,-coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{ -coll.x, coll.y,-coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{  coll.x,-coll.y,-coll.z, 1.0 }},
		p + glm::vec3{M * glm::vec4{ -coll.x,-coll.y,-coll.z, 1.0 }}
	};

	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 9; i++) {
			if ((pos[i].x < b.x && a.x < pos[i].x) && \
				(pos[i].y < b.y && a.y < pos[i].y) && \
				(pos[i].z < b.z && a.z < pos[i].z)) {
				return true;
			}
			pos[i] += nn;
		}
	}
	return false;
}

void PLANE::update() {
	//ver 2 go view 순서
	this->go();
	this->view();

	this->set_speed(-0.03f);
	this->update_head_light();
	this->check_horizon();

	this->setPos();
	this->reRoll();
}



////////
///

