#include"Header.h"
#include"light.h"
#include"Obj.h"
#include"texture_sunkue.h"
#include"shader_sunkue.h"
#include"camera.h"
#include"PLANE.h"
#include"screen.h"
#include"Alpha_blending.h"
#include"debug.h"
#include"Sound.h"

constexpr int FPS{ 60 };
constexpr int IM{ 9 };
constexpr int main_timer{ 0 };
constexpr int monsternum_by_col{ 3 };

int monsternum{ static_cast<int>(COLOR_::count) * monsternum_by_col };
/*Funcs*/
GLvoid drawScene(GLvoid);
/**/
GLvoid Reshape(int w, int h);
GLvoid SpecialInput(int key, int x, int y);
GLvoid SpecialInput_up(int key, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid keyboard_up(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseWheel(int button, int dir, int x, int y);
GLvoid Timer(int value);
/**/
GLvoid print_message();
GLvoid DefaultObj();
GLvoid MakeShape();
GLvoid MakeIM();
GLvoid SetSound();

/**/
struct bullet_ {
	Obj obj;
	glm::vec3 dir;
	unsigned int life{ 100 };
};
struct monster_ {
	Obj obj;
	COLOR_ color_type;
};

constexpr int score_minimum{ 3 };
struct score_ {
	Obj obj;
	glm::vec3 pos;
	int num;

	void init() {
		obj = Obj();
		LoadObj("cube.txt", obj, "8/8/8");
		obj.M.resize(3, df);
		obj.M.at(2) = glm::scale(df, glm::vec3(1.15f));

		pos = plane.pos;

		set_num();
		update();
	}

	void set_num() {
		constexpr int d = 6 - score_minimum;
		num = rand() % d + score_minimum;
		update_tex();
	}

	bool add_num(int a) {
		num += a;
		if (num <= 0) {
			set_num();
			return false;
		}
		else if (6 < num) {
			num = 6;
			update_tex();
			return false;
		}
		else {
			update_tex();
			return true;
		}
	}

	void set_pos(glm::vec3 a) {
		pos = a;
	}

	void update_tex() {
		switch (num)
		{
		case 1: {
			LoadTexture(obj, "face1.jpg", 512, 512, 1);
			break;
		}
		case 2: {
			LoadTexture(obj, "face2.jpg", 512, 512, 1);
			break;
		}
		case 3: {
			LoadTexture(obj, "face3.jpg", 512, 512, 1);
			break;
		}
		case 4: {
			LoadTexture(obj, "face4.jpg", 512, 512, 1);
			break;
		}
		case 5: {
			LoadTexture(obj, "face5.jpg", 512, 512, 1);
			break;
		}
		case 6: {
			LoadTexture(obj, "face6.jpg", 512, 512, 1);
			break;
		}
		default:
			std::cout << "score target error\n";
			break;
		}
	}

	void update() {
		obj.M.at(0) = glm::translate(df, pos);
		constexpr GLfloat rad = glm::radians(5.0f);
		obj.M.at(1) *= glm::rotate(df, rad, { 0.0,1.0,0.0 });
	}

};
/**/
Obj world;
Obj ground[IM], building[IM][buildingnum], cloud[IM][cloudnum];
std::vector<monster_> monster[IM];
monster_ tempmonster;
LIGHT sun, moon;
std::vector<bullet_> bullet;
score_ score;

CSound* day, * night, * wind, * sound_shooting, * sound_item, * sound_item_fail, * sound_explosion;

/*-----MAIN--*/
int main(int argc, char** argv) {
	srand((unsigned int)time(NULL));
	/* window */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(screen.width, screen.height);
	int winID = glutCreateWindow("OPEN GL");
	/* op gl */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	if (!GLEW_VERSION_3_3) {

		std::cerr << "Error: OpenGL 3.3 API is not available.\n";
		std::exit(EXIT_FAILURE);
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "\n\n\n";

	/* glsl shader */
	shaderID = InitShader();
	if (!shaderID) {
		std::cerr << "Error: Shader Program ???? ????\n";
		std::exit(EXIT_FAILURE);
	}
	/* uniform */
	init_uniform_Loc();

	/* states */
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Funcs */
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutSpecialFunc(SpecialInput);
	glutSpecialUpFunc(SpecialInput_up);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutMouseFunc(Mouse);
	glutMouseWheelFunc(MouseWheel);

	/* set timer */
	
	glutTimerFunc(1200 / FPS, Timer, main_timer);

	/* setting */
	set_flip_texture(true);
	DefaultObj();
	MakeShape();
	print_message();
	SetSound();
	/* Loop */
	glutMainLoop();
	std::cout << "mainLoop error";
	CSound::Release();
}
GLvoid print_message() {
	std::cout <<
		"\
		-= -= -= -= -= -= -= -= -= -= -= -= -= -= -\n\
		head_light [ 1 ]\n\
		head_light_degree [ Rclick + mouse_wheel ]\n\
		plane[w a s d up(8) down(5) left(4) right(6)]\n\
		camera[i j k l  mouse_wheel 2 c ]\n\
		-= -= -= -= -= -= dev = -= -= -= -= -= -= -\n\
		Change_plane_color[ L_Ctrl ]\n\
		time_stop_SM [ z ]\n\
		fire[ space ]\n\
		=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
}
GLvoid DefaultObj() {
	/*temp sphere*/


	/*world*/
	LoadObj("sphere.obj", world, "8/8/8");
	LoadTexture(world, "skydome.jpg", 2048, 1024, 3);
	world.M.push_back(glm::scale(df, screen.size_of_world * 2000.0f));
	world.Reverse_nor();

	/*light*/
	LoadObj("sphere.obj", sun.obj, "8/8/8");
	sun.obj.Set_Color({ 10.0, 5.0, 5.0, 1.0 });
	sun.pos = { groundsize * 3.0f, 0.0, 0.0 };
	sun.col = { 100.0,100.0,100.0 };
	sun.obj.M.resize(2, df);
	sun.obj.M.push_back(glm::scale(df, glm::vec3(80.0f)));



	LoadObj("sphere.obj", moon.obj, "8/8/8");
	moon.obj.Set_Color({ 1.0, 1.0, 10.0, 1.0 });
	moon.pos = { groundsize * -3.0f, 0.0, 0.0 };
	moon.col = { 0.5,0.5,100.0 };
	moon.obj.M.resize(2, df);
	moon.obj.M.push_back(glm::scale(df, glm::vec3(40.0f)));
}
GLvoid MakeShape() {
	plane.init();
	score.init();
	{
		Obj tempbuilding;
		LoadObj("apartment.obj", tempbuilding, "8/8/8");
		tempbuilding.M.resize(3, df);
		tempbuilding.M.at(2) = glm::scale(df, glm::vec3(10.0f));
		//tempbuilding.objData.shine = 640;

		for (int i = 0; i < buildingnum; i++) {
			building[0][i] = tempbuilding;
			building[0][i].Set_Color({ 0.5f,0.5f,GLfloat(rand() % 10) / 10.0f,1.0f });
			building[0][i].M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
			building[0][i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize) * 2) - groundsize,ground_floor,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
		//std::cout << "==building==\n";
		//cout_coll_box(building[0][0]);
	}
	{
		Obj tempcloud;
		LoadObj("cloud.obj", tempcloud, "8/8/8");
		tempcloud.Set_Color({ 1.0f,1.0f,1.0f,0.5f });
		tempcloud.M.resize(3, df);
		tempcloud.M.at(2) = glm::scale(df, glm::vec3(1.0f));
		for (int i = 0; i < cloudnum; i++) {
			cloud[0][i] = tempcloud;
			cloud[0][i].M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
			cloud[0][i].M.at(0) = glm::translate(df, { GLfloat(rand() % (int(groundsize) * 2)) - groundsize,ground_floor + GLfloat(rand() % 4000) + 1000.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
	}
	{
		tempmonster = monster_();
		LoadObj("monster.obj", tempmonster.obj, "8/8/8");
		tempmonster.obj.M.resize(3, df);
		tempmonster.obj.M.at(2) = glm::scale(df, glm::vec3(20.0f));
		tempmonster.obj.objData.shine = 640;
		//tempmonster.obj.objData.vertices.clear();

		monster[0].resize(monsternum);
		monster[0].reserve(6);
		COLOR_ tC{ COLOR_::RED };
		int temp{ 0 };
		for (int i = 0; i < monsternum; i++) {
			monster[0].at(i) = tempmonster;
			monster[0][i].obj.M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
			monster[0][i].obj.M.at(0) = glm::translate(df, { GLfloat(rand() % (int(groundsize) * 2)) - groundsize,ground_floor + GLfloat(rand() % 800) + 1800.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
			if (temp == monsternum_by_col) {
				tC++;
				temp = 0;
			}
			monster[0][i].color_type = tC;
			//std::cout << temp << '\n';
			ChangeCol(monster[0][i].obj, monster[0][i].color_type);
			temp++;
		}

		for (int i = 0; i < score.num - 3; i++) {
			monster[0].push_back(tempmonster);
			monster[0].back().obj.M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
			monster[0].back().obj.M.at(0) = glm::translate(df, { GLfloat(rand() % (int(groundsize) * 2)) - groundsize,ground_floor + GLfloat(rand() % 800) + 1800.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
			monster[0].back().color_type = plane.color_type;
			ChangeCol(monster[0].back().obj, monster[0].back().color_type);
		}
		//std::cout << "==monster==\n";
		//cout_coll_box(monster[0][0].obj);
	}
	{
		LoadObj("cube.txt", ground[0], "8/8/8");
		LoadTexture(ground[0], "sea.jpg", 512, 512, 3);
		ground[0].M.push_back(glm::translate(df, { 0.0, ground_floor,0.0 }));
		ground[0].M.push_back(glm::scale(df, { groundsize,10.0,groundsize }));
	}
	MakeIM();
};
GLvoid MakeIM() {
	/*
			1 2 3
		^	4 0 5
		z	6 7 8
			x >

	*/
	constexpr GLfloat size{ groundsize * 2.0f };
	glm::mat4 tr[IM]{
		glm::mat4(1.0f),
		glm::translate(df,{-size,0.0,size}),
		glm::translate(df,{0.0,0.0,size}),
		glm::translate(df,{size,0.0,size}),

		glm::translate(df,{-size,0.0,0.0}),
		glm::translate(df,{size,0.0,0.0}),

		glm::translate(df,{-size,0.0,-size}),
		glm::translate(df,{0.0,0.0,-size}),
		glm::translate(df,{size,0.0,-size}),
	};

	for (int im = 1; im < IM; im++) {
		for (int j = 0; j < buildingnum; j++) {
			building[im][j] = building[0][j];
			building[im][j].M.at(0) *= tr[im];
		}
	}
	for (int im = 1; im < IM; im++) {
		for (int j = 0; j < cloudnum; j++) {
			cloud[im][j] = cloud[0][j];
			cloud[im][j].M.at(0) *= tr[im];
		}
	}
	for (int im = 1; im < IM; im++) {
		monster[im] = monster[0];
		for (int j = 0; j < monster[0].size(); j++) {
			monster[im][j].obj.M.at(0) *= tr[im];
		}
	}
	for (int im = 1; im < IM; im++) {
		ground[im] = ground[0];
		ground[im].M.at(0) *= tr[im];
	}
}
GLvoid SetSound() {
	CSound::Init();
	day = new CSound("res/sound/day.mp3", true);
	night = new CSound("res/sound/night.mp3", true);
	wind = new CSound("res/sound/wind01.mp3", true);
	sound_shooting = new CSound("res/sound/shooting.wav", false);
	sound_item = new CSound("res/sound/item.mp3", false);
	sound_item_fail = new CSound("res/sound/item_fail.mp3", false);
	sound_explosion = new CSound("res/sound/explosion.wav", false);

	day->play();
	night->play();
	wind->play();
}


bool cull_this_in_proj(Obj& obj, glm::mat4& PV) {
	// ???????? ???? ???? ????, ???????????? ???? ????.
	glm::vec3 pivot = obj.objData.vertices.at(1).pos;
	glm::vec4 gl_pos = PV * obj.world_M() * glm::vec4(pivot, 1.0f);
	gl_pos = gl_pos / gl_pos.w;
	if (1.0f < abs(gl_pos.x))return true;
	else if (1.0f < abs(gl_pos.y))return true;
	else if (1.0f < gl_pos.z)return true;
	else return false;
}
/*?????? ????*/
GLvoid Set_draw() {
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.view_M()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(screen.proj_M()));
	LIGHT::init_light_buffer();
	glUniform3fv(lightPosLoc, LIGHT::light_num, LIGHT::lights_pos);
	glUniform3fv(lightColLoc, LIGHT::light_num, LIGHT::lights_col);
	glUniform1iv(use_spotLoc, LIGHT::light_num, LIGHT::lights_use_spot);
	glUniform1fv(spot_cosLoc, LIGHT::light_num, LIGHT::lights_spot_cos);
	glUniform1fv(spot_out_cosLoc, LIGHT::light_num, LIGHT::lights_spot_out_cos);
	glUniform3fv(spot_dirLoc, LIGHT::light_num, LIGHT::lights_spot_dir);

	glUniform3f(ambientColorLoc, LIGHT::ambientColor.r, LIGHT::ambientColor.g, LIGHT::ambientColor.b);
	glUniform3f(viewPosLoc, camera.EYE.x, camera.EYE.y, camera.EYE.z);
	glUniform1f(ambientLoc, LIGHT::ambient);
	glUniform3f(specLoc, sun.spec.r, sun.spec.g, sun.spec.b);
	glUniform1i(texLoc, 0);
	glUniform1i(light_numLoc, LIGHT::light_num);
}
GLvoid drawScene() {
	/*???? ????*/
	Set_draw();

	/*?????? ????*/
	std::vector<Obj*> Alpha_objs;
	Alpha_objs.reserve(16);
	{
		glFrontFace(GL_CW);
		drawObj(world);
		glFrontFace(GL_CCW);
		drawObj(plane.obj);
		drawObj(score.obj);
		/*??????*/
		drawObj(sun.obj);
		drawObj(moon.obj);
		for (int im = 0; im < IM; im++) {
			drawObj(ground[im]);
		}
		for (int im = 0; im < IM; im++) {
			for (int i = 0; i < buildingnum; i++) {
				drawObj(building[im][i]);
			}
		}
		for (int im = 0; im < IM; im++) {
			for (std::vector<monster_>::iterator i{ monster[im].begin() }, e{ monster[im].end() }; i != e; i++) {
				drawObj(i->obj);
			}
		}

		for (std::vector<bullet_>::iterator i = bullet.begin(), e = bullet.end(); i != e; i++) {
			drawObj(i->obj);
		}
	}
	/*alpha*/
	/*???? ALpha_objs ?? push ???? ?????? ?? ?????? ??*/
	{
		for (int im = 0; im < IM; im++) {
			for (int i = 0; i < cloudnum; i++) {
				drawObj(cloud[im][i]);
			}
		}
		/*
		???????? ?????? ?????? ???? ????.

		glm::mat4 PV = df * screen.proj_M() * camera.view_M();
		for (int i = 0; i < cloudnum; i++) {
			if (!cull_this_in_proj(cloud[i], PV)) {
				Alpha_objs.push_back(&cloud[i]);
			}
		}
		*/
	}
	/*?????? ??*/
	Sort_Alpha_blending(Alpha_objs);
	Draw_Alpha_blending(Alpha_objs);

	glutSwapBuffers();
}

/*?????? ????*/

int shot_delay = 0, sound_delay = 0;
bool P_go, P_stop, P_YL, P_YR, P_RL, P_RR, P_PU, P_PD, timeStop, stealth, bl_shot;
bool up, down, left, right, L_drag, R_drag, view_SM;
GLvoid Timer(int value) {
	constexpr GLfloat degree{ 5.0f };
	static GLfloat day__{ 0.0f }, night__{ 0.0f };

	switch (value)
	{
	case  main_timer: {
		/* shot */
		{
			shot_delay = (shot_delay + 1) % 5;
			if (shot_delay == 0) {
				if (bl_shot) {
					sound_delay = (sound_delay + 1) % 2;
					if (sound_delay == 0) {
						sound_shooting->play();
						//std::cout << "sound_shooting-play\n";
					}
					bullet.push_back(bullet_());;
					bullet_* a = &(bullet.back());
					LoadObj("sphere.obj", a->obj, "8/8/8");
					a->obj.M.resize(3, df);
					// ???? ???? = ???? ????.
					a->obj.M[2] = glm::scale(df, { 0.1,0.1,1.0 });
					// ???? ???? == ???? ????.
					a->obj.M[1] = plane.obj.M[1];
					a->obj.M[0] = plane.obj.M[0];
					a->obj.Set_Color({ 100.0f,0.4f,0.4f,1.0f });

					a->dir = -1.0f * plane.nDir();
					a->obj.M[0] *= glm::translate(df, a->dir);
					//update ???? ?? b->obj.M[0] = plane.obj.M[0] * glm::translate(df, { plane.nDir() * -3.0f });
				}
			}
			std::vector<std::vector<bullet_>::iterator> trashcan;
			for (std::vector<bullet_>::iterator i = bullet.begin(), e = bullet.end(); i != e; i++) {
				i->obj.M[0] *= glm::translate(df, i->dir * (plane.maxspeed + 10.0f));
				i->life -= 1;
				if (i->life == 0) {
					trashcan.reserve(1);
					trashcan.push_back(i);
				}
			}
			for (std::vector<std::vector<bullet_>::iterator>::iterator i = trashcan.begin(), e = trashcan.end(); i != e; i++) {
				(*i)->obj.DelObj();
				bullet.erase(*i);
			}
		}
		/* plane */
		{
			if (P_go) {
				plane.set_speed(0.64f);
			}
			if (P_stop) {
				plane.set_speed(-2.0f);
			}
			if (P_YL) {
				//ver 1
				plane.Yaw(degree / (FPS / 6));
			}
			if (P_YR) {
				//ver 1
				plane.Yaw(-degree / (FPS / 6));
			}
			if (P_RL) {
				plane.Roll(degree / (FPS / 24));
				//plane.Pitch(degree / (FPS / 4.8));
			}
			if (P_RR) {
				plane.Roll(-degree / (FPS / 24));
				//plane.Pitch(degree / (FPS / 4.8));
			}
			if (P_PU) {
				plane.Pitch(degree / (FPS / 4.8));
			}
			if (P_PD) {
				plane.Pitch(-degree / (FPS / 4.8));
			}
			if (stealth) {
				constexpr int tm{ 50 };
				constexpr int tm2{ 25 };
				static int time{ tm };
				if (tm2 < time)plane.Stealth(true);
				else if (time % 2) {
					plane.Stealth(true);
				}
				else {
					plane.Stealth(false);
				}
				time--;
				if (time < 0)stealth = false, time = tm;
			}
			plane.update();
		}
		/* check_coll */
		{
			//timer::start_mes(); //5-7 // coll-> [10 < ] ms
			if (!stealth) {
				{
					constexpr glm::vec4 a{ -32.4,18.2,-12.5 ,1.0f }, b{ 32.4,115.33,18.63 ,1.0f };
					for (int i = 0; i < buildingnum; i++) {
						glm::mat4 M{ building[0][i].world_M() };
						if (plane.check_coll(M * a, M * b)) {
							sound_explosion->play();
							//std::cout << "sound_explosion-play\n";
							plane.setPos({ 0.0, 0.0, 0.0 });
							plane.set_speed(-plane.speed + 2.0f);
							stealth = true;
							break;
						}
					}
				}
				{
					constexpr glm::vec4 a{ -13.0,-15.7,-5.2 ,1.0f }, b{ 13.0,20.0,13.5,1.0f };
					std::vector<std::vector<monster_>::iterator> trashcan[IM];
					bool add{ false }, add1{ false };
					for (std::vector<monster_>::iterator i{ monster[0].begin() }, e{ monster[0].end() }; i != e; i++) {
						glm::mat4 M{ i->obj.world_M() };
						if (plane.check_coll(M * a, M * b)) {

							if (plane.color_type == i->color_type) {
								sound_item->play();
								//std::cout << "sound_item-play\n";
								if (!score.add_num(-1)) {
									//change color
									constexpr int d = static_cast<int>(COLOR_::count) - 1;
									int x = rand() % d + 1;
									for (int i = 0; i < x; i++)	plane.color_type++;
									ChangeCol(plane.obj, plane.color_type);

									add = true;
								}
							}
							else {
								stealth = true;
								sound_item_fail->play();
								//std::cout << "sound_item_fail-play\n";
								if (score.add_num(1))add1 = true;
							}

							for (int im = 0; im < IM; im++) {
								trashcan[im].push_back(monster[im].begin() + (i - monster[0].begin()));
							}


							break;
						}
					}
					// trashcan
					for (int im = 0; im < IM; im++) {
						for (std::vector<std::vector<monster_>::iterator>::iterator i = trashcan[im].begin(), e = trashcan[im].end(); i != e; i++) {
							monster[im].erase(*i);

						}
					}
					//add monster
					{
						constexpr GLfloat size{ groundsize * 2.0f };
						static glm::mat4 tr[IM]{
							glm::mat4(1.0f),
							glm::translate(df,{-size,0.0,size}),
							glm::translate(df,{0.0,0.0,size}),
							glm::translate(df,{size,0.0,size}),

							glm::translate(df,{-size,0.0,0.0}),
							glm::translate(df,{size,0.0,0.0}),

							glm::translate(df,{-size,0.0,-size}),
							glm::translate(df,{0.0,0.0,-size}),
							glm::translate(df,{size,0.0,-size}),
						};

						if (add) {
							monster[0].reserve(score.num);
							for (int i = 0; i < score.num; i++) {
								monster[0].push_back(tempmonster);
								monster[0].back().obj.M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
								monster[0].back().obj.M.at(0) = glm::translate(df, { GLfloat(rand() % (int(groundsize) * 2)) - groundsize,ground_floor + GLfloat(rand() % 800) + 1800.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
								monster[0].back().color_type = plane.color_type;
								ChangeCol(monster[0].back().obj, monster[0].back().color_type);
							}
							for (int im = 1; im < IM; im++) {
								monster[im] = monster[0];
								for (int j = 0; j < monster[0].size(); j++) {
									monster[im][j].obj.M.at(0) *= tr[im];
								}
							}
						}

						if (add1) {
							monster[0].reserve(1);
							monster[0].push_back(tempmonster);
							monster[0].back().obj.M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
							monster[0].back().obj.M.at(0) = glm::translate(df, { GLfloat(rand() % (int(groundsize) * 2)) - groundsize,ground_floor + GLfloat(rand() % 800) + 1800.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
							monster[0].back().color_type = plane.color_type;
							ChangeCol(monster[0].back().obj, monster[0].back().color_type);

							constexpr GLfloat size{ groundsize * 2.0f };
							for (int im = 1; im < IM; im++) {
								monster[im].reserve(1);
								monster[im].push_back(monster[0].back());
								monster[im].back().obj.M.at(0) *= tr[im];
							}
						}
					}
				}
				//timer::end_mes();
			}
		}
		/* day, whether */
		{
			//asf
			if (timeStop == false) {
				constexpr GLfloat light_degree{ -1.0f / (FPS / 24.0f) };
				sun.pos = glm::rotate(df, glm::radians(light_degree), { 0.0,0.0,1.0 }) * glm::vec4{ sun.pos ,1.0 };
				sun.update();
				if (ground_floor < sun.pos.y) {
					GLfloat d = sun.pos.y / groundsize;
					LIGHT::ambient = d;
					LIGHT::ambientColor = { 1.0,d,1.0 };
					night__ = 0.0f;
					day__ += abs(light_degree);
					sun.on();
				}
				else sun.off();

				moon.pos = glm::rotate(df, glm::radians(light_degree), { 0.0,0.0,1.0 }) * glm::vec4{ moon.pos ,1.0 };
				moon.update();
				if (ground_floor < moon.pos.y) {
					LIGHT::ambient = 0.6f;
					LIGHT::ambientColor = { 0.3,0.3,1.0 };
					day__ = 0.0f;
					night__ += abs(light_degree);
					moon.on();
				}
				else moon.off();
			}
			//asdf
		}
		/* score */
		{
			static GLfloat d = 0.0;
			d += 0.2f;
			glm::vec3 D{ plane.Right() };
			D.y = 0.0f;
			D = glm::normalize(D);
			score.set_pos(plane.pos + plane.nDir() * 18.0f + plane.grav * 5.0f + D * 2.0f * sin(d));
			score.update();
		}
		/* camera */
		{
			if (view_SM) {
				glm::vec3 dir;
				if (day__ != 0.0f) {
					camera.AT = sun.pos;
					dir = glm::normalize(plane.pos - sun.pos);
				}
				if (night__ != 0.0f) {
					camera.AT = moon.pos;
					dir = glm::normalize(plane.pos - moon.pos);
				}
				constexpr glm::mat4 M{
					0.951187,	-0.0153978,	0.308231,	0,
					-0.0334187,	0.987743,	-0.152471,	0,
					0.306801,	0.155329,	0.939014,	0,
					0,	0,	0,	1
				};
				camera.EYE = glm::translate(df, plane.pos) * M * glm::translate(df, dir * plane.view_dist) * glm::vec4(1.0f);
			}
			else {
				if (up) {
					glm::mat4 R = glm::rotate(df, glm::radians(-degree / (FPS / 12)), camera.Right());
					plane.viewMat = R * plane.viewMat;
					camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
				}
				if (down) {
					glm::mat4 R = glm::rotate(df, glm::radians(degree / (FPS / 12)), camera.Right());
					plane.viewMat = R * plane.viewMat;
					camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
				}
				if (right) {
					glm::mat4 R = glm::rotate(df, glm::radians(degree / (FPS / 12)), camera.Up());
					plane.viewMat = R * plane.viewMat;
					camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
				}
				if (left) {
					glm::mat4 R = glm::rotate(df, glm::radians(-degree / (FPS / 12)), camera.Up());
					plane.viewMat = R * plane.viewMat;
					camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
				}
			}
		}
		/* sound */
		{
			day->setVolume(glm::sin(glm::radians(day__)));
			day->Update();
			night->setVolume(glm::sin(glm::radians(night__)));
			night->Update();
			wind->setVolume(plane.speed / plane.maxspeed);
			wind->Update();
			sound_shooting->Update();
			sound_item->Update();
			sound_item_fail->Update();
			sound_explosion->Update();
		}
		glutTimerFunc(1200 / FPS, Timer, value);
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (button) {
	case GLUT_LEFT_BUTTON: {
		switch (state)
		{
		case GLUT_UP: {
			L_drag = false;
			break;
		}
		case GLUT_DOWN: {
			L_drag = true;
			break;
		}
		default: { break; }
		}
		break;
	}
	case GLUT_RIGHT_BUTTON: {
		switch (state)
		{
		case GLUT_UP: {
			R_drag = false;
			break;
		}
		case GLUT_DOWN: {
			R_drag = true;
			break;
		}
		default: { break; }
		}
		break;
	}
	case GLUT_MIDDLE_BUTTON: {
		switch (state) {
		case GLUT_UP: {
			break;
		}
		case GLUT_DOWN: {
			break;
		}
		default: { break; }
		}
		break;
	}
	}
}
GLvoid MouseWheel(int button, int dir, int x, int y) {
	constexpr GLfloat dd{ 0.04f };
	if (dir > 0)
	{
		if (R_drag) {
			plane.head_light.spot_theta += dd;
			if (4.25f < plane.head_light.spot_theta) { plane.head_light.spot_theta = 4.25f; }
		}
		else {
			plane.view_dist_add(-10.0f);
		}
	}
	else
	{
		if (R_drag) {
			plane.head_light.spot_theta -= dd;
			if (plane.head_light.spot_theta < 1.0f) { plane.head_light.spot_theta = 1.0f; }
		}
		else {
			plane.view_dist_add(10.0f);
		}
	}
	//std::cout << plane.head_light.spot_theta << '\n';
	glutPostRedisplay();
}
GLvoid SpecialInput(int key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case GLUT_KEY_UP: {
		P_PU = true;
		break;
	}
	case GLUT_KEY_DOWN: {
		P_PD = true;
		break;
	}
	case GLUT_KEY_RIGHT: {
		P_RR = true;
		break;
	}
	case GLUT_KEY_LEFT: {
		P_RL = true;
		break;
	}
	case GLUT_KEY_CTRL_L: {
		plane.color_type++;
		ChangeCol(plane.obj, plane.color_type);
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid SpecialInput_up(int key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case GLUT_KEY_UP: {
		P_PU = false;
		break;
	}
	case GLUT_KEY_DOWN: {
		P_PD = false;
		break;
	}
	case GLUT_KEY_RIGHT: {
		P_RR = false;
		break;
	}
	case GLUT_KEY_LEFT: {
		P_RL = false;
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case '`': {
		if (!stealth) {
			stealth = true;
		}
		break;
	}
	case 'i': {
		up = true;
		break;
	}
	case 'k': {
		down = true;
		break;
	}
	case 'j': {
		left = true;
		break;
	}
	case 'l': {
		right = true;
		break;
	}
	case '1': {
		plane.HeadLightOnOff();
		plane.head_light.spot_theta = 2.5f;
		// plane.head_light.col = glm::vec3(1.0f);
		break;
	}
	case '2': {
		view_SM = true;
		break;
	}
	case 'C':
	case 'c': {
		plane.viewMat = df;
		camera.UP = { 0.0,1.0,0.0 };
		break;
	}
	case 'W':
	case 'w': {
		P_go = true;
		break;
	}
	case 'S':
	case 's': {
		P_stop = true;
		break;
	}
	case 'A':
	case 'a': {
		P_YL = true;
		break;
	}
	case 'D':
	case 'd': {
		P_YR = true;
		break;
	}
	case '4': {
		P_RL = true;
		break;
	}
	case '6': {
		P_RR = true;
		break;
	}
	case '8': {
		P_PD = true;
		break;
	}
	case '5': {
		P_PU = true;
		break;
	}
	case 'Z':
	case 'z': {
		timeStop = timeStop ? false : true;
		break;
	}
	case ' ': {
		bl_shot = true;
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid keyboard_up(unsigned char key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case 'i': {
		up = false;
		break;
	}
	case 'k': {
		down = false;
		break;
	}
	case 'j': {
		left = false;
		break;
	}
	case 'l': {
		right = false;
		break;
	}
	case 'W':
	case 'w': {
		P_go = false;
		break;
	}
	case 'S':
	case 's': {
		P_stop = false;
		break;
	}
	case 'A':
	case 'a': {
		P_YL = false;
		break;
	}
	case 'D':
	case 'd': {
		P_YR = false;
		break;
	}
	case '2': {
		view_SM = false;
		break;
	}
	case '4': {
		P_RL = false;
		break;
	}
	case '6': {
		P_RR = false;
		break;
	}
	case '8': {
		P_PD = false;
		break;
	}
	case '5': {
		P_PU = false;
		break;
	}
	case ' ': {
		bl_shot = false;
		break;
	}
	default:
		break;
	}
	glutPostRedisplay();
}
GLvoid Reshape(int w, int h) {
	screen.width = w, screen.height = h;
	glViewport(0, 0, screen.width, screen.height);
}