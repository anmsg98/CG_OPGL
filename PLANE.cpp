#include"PLANE.h"

PLANE plane;

	void PLANE::init() {
		// 0 Trans	1 Yaw	2 Pitch	3 Roll	4 Size
		this->obj.M.resize(3, df);
		LoadObj("airplane.obj", this->obj, "8/8/8");
		this->obj.M.at(2) = glm::scale(df, glm::vec3(0.25f));
		this->obj.M.at(1) = glm::rotate(df, glm::radians(180.0f), { 0.0,1.0,0.0 });
		this->setPos();
		this->default_color();
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

	glm::mat4 PLANE::yaw_(GLfloat degree) {
		return glm::rotate(df, glm::radians(degree), Up());
	}
	glm::mat4 PLANE::pitch_(GLfloat degree) {
		return glm::rotate(df, glm::radians(degree), Right());
	}
	glm::mat4 PLANE::roll_(GLfloat degree) {
		return glm::rotate(df, glm::radians(degree), nDir());
	}

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

	void PLANE::go() {
		glm::mat4 m = glm::translate(df, nDir() * -speed);
		this->pos = m * glm::vec4{ this->pos,1.0f };
		check_area();
	}

	void PLANE::check_area() {
		/*�ٴ� üũ*/
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
		glm::mat4 m;
		switch (eye_mode)
		{
		case 0: {m = glm::translate(df, nDir() * 100.0f); break; }
		case 1: {m = glm::translate(df, nDir() * 400.0f); break; }
		case 2: {m = glm::translate(df, Right() * 5.0f) * glm::translate(df, nDir() * 10.0f); break; }
		}
		camera.AT = glm::translate(df, nDir() * -15.0f) * glm::vec4(this->pos, 1.0f);
		camera.EYE = m * glm::vec4{ this->pos ,1.0f };
		if (camera.EYE.y < ground_floor + 20.0f)camera.EYE.y = ground_floor + 20.0f;
	}

	void PLANE::update(bool s) {
		if (!s)this->view();
		this->go();
		this->setPos();
	}
