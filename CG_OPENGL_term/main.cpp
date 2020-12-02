#include"Header.h"
#include"light.h"
#include"Obj.h"
#include"texture_sunkue.h"
#include"shader_sunkue.h"
#include"camera.h"
#include"PLANE.h"
#include"screen.h"
#include"Alpha_blending.h"

#define FLYING 1
#define FPS 60 

constexpr int IM{ 9 };

/*Funcs*/
GLvoid drawScene(GLvoid);
/**/
GLvoid Reshape(int w, int h);
GLvoid SpecialInput(int key, int x, int y);
GLvoid SpecialInput_up(int key, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid keyboard_up(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid MouseWheel(int button, int dir, int x, int y);
GLvoid Timer(int value);
/**/
GLvoid MakeShape();
GLvoid DefaultObj();
GLvoid print_message();
GLvoid Reserve_IM();
/**/
struct bullet_ {
	Obj obj;
	glm::vec3 dir;
	unsigned int life{ 100 }; 
};
Obj world;
Obj ground, building[buildingnum], cloud[cloudnum];
LIGHT sun, moon;
std::vector<bullet_> bullet;

std::vector<Obj*> MAP[IM];

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
		std::cerr << "Error: Shader Program 생성 실패\n";
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
	glutMotionFunc(Motion);
	glutMouseWheelFunc(MouseWheel);

	/* set timer */
	glutTimerFunc(50, Timer, 0);
	glutTimerFunc(12000 / FPS, Timer, 1);

	/* setting */
	set_flip_texture(true);
	DefaultObj();
	MakeShape();
	print_message();
	Reserve_IM();

	/* Loop */
	glutMainLoop();
}
GLvoid print_message() {
	std::cout <<
		"\
		-= -= -= -= -= -= -= -= -= -= -= -= -= -= -\n\
		time_stop[ z ]\n\
		fire[ space ]\n\
		head_light [ 1 2 3 ]\n\
		plane[w a s d up(8) down(5) left(4) right(6)]\n\
		rebuilding[tab]\n\
		camera[i j k l  mouse_wheel]\n\
		=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\
		";
}
GLvoid DefaultObj() {
	/*temp sphere*/


	/*world*/
	LoadObj("sphere.obj", world, "8/8/8");
	LoadTexture(world, "skydome.jpg", 2048, 1024, 3);
	world.M.push_back(glm::scale(df, screen.size_of_world*100.0f));
	world.Reverse_nor();

	/*light*/
	LoadObj("sphere.obj", sun.obj, "8/8/8");
	sun.obj.Set_Color({ 10.0, 5.0, 5.0, 1.0 });
	sun.pos = { groundsize, 0.0, 0.0 };
	sun.col = { 100.0,100.0,100.0 };
	sun.obj.M.resize(2, df);
	sun.obj.M.push_back(glm::scale(df, glm::vec3(80.0f)));



	LoadObj("sphere.obj", moon.obj, "8/8/8");
	moon.obj.Set_Color({ 1.0, 1.0, 10.0, 1.0 });
	moon.pos = { -groundsize, 0.0, 0.0 };
	moon.col = { 0.5,0.5,100.0 };
	moon.obj.M.resize(2, df);
	moon.obj.M.push_back(glm::scale(df, glm::vec3(40.0f)));
}
GLvoid MakeShape() {
	plane.init();
	{
		for (int i = 0; i < buildingnum; i++) {
			LoadObj("apartment.obj", building[i], "8/8/8");
			building[i].Set_Color({ 0.5f,0.5f,GLfloat(rand() % 10) / 10.0f,1.0f });
			building[i].M.resize(3, df);
			building[i].M.at(2) = glm::scale(df, glm::vec3(10.0f));
			building[i].M.at(1) = glm::rotate(df,glm::radians(GLfloat(rand()%360)), {0.0,1.0,0.0});
			building[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize)*2) - groundsize,ground_floor,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
	}
	{
		for (int i = 0; i < cloudnum; i++) {
			LoadObj("cloud.obj", cloud[i], "8/8/8");
			cloud[i].Set_Color({ 1.0f,1.0f,1.0f,0.5f });
			cloud[i].M.resize(3, df);
			cloud[i].M.at(2) = glm::scale(df, glm::vec3(1.0f));
			cloud[i].M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
			cloud[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize) * 2) - groundsize,ground_floor + GLfloat(rand()%800)+1800.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
	}
	{
		LoadObj("cube.txt", ground, "8/8/8");
		LoadTexture(ground, "grass.jpg", 512, 512, 3);
		ground.M.push_back(glm::translate(df, { 0.0, ground_floor,0.0 }));
		ground.M.push_back(glm::scale(df, { groundsize,10.0,groundsize }));
	}
	/**/
};
GLvoid Reserve_IM() {
	size_t objss{ buildingnum + cloudnum + 1 };
	for (int i = 0; i < IM; i++) {
		MAP[i].reserve(objss);
	}
}

bool cull_this_in_proj(Obj& obj, glm::mat4& PV) {
	// 속도떔시 대충 만든 코드, 부정확하지만 일단 사용.
	glm::vec3 pivot = obj.objData.vertices.at(1).pos;
	glm::vec4 gl_pos = PV * obj.world_M() * glm::vec4(pivot, 1.0f);
	gl_pos = gl_pos / gl_pos.w;
	if (1.0f < abs(gl_pos.x))return true;
	else if (1.0f < abs(gl_pos.y))return true;	
	else if (1.0f < gl_pos.z)return true;
	else return false;
}
/*그리기 함수*/
GLvoid Set_IM() {
	/* ground, building, cloud */
	MAP[0].push_back(&ground);

	for (int i = 0; i < buildingnum; i++) {
		MAP[0].push_back(&building[i]);
	}

	for (int i = 0; i < cloudnum; i++) {
		MAP[0].push_back(&cloud[i]);
	}
}
GLvoid Make_IM() {
	/*
	
			1 2 3
		^	4 0 5
		z	6 7 8
			x >
	
	*/


	// MAP copy
	for (int i = 1; i < IM; i++) {
		MAP[i] = MAP[0];
	}

	// MAP trans
	glm::mat4 trans[IM]{
		glm::mat4(1.0f),

		glm::translate(df,{-groundsize,0.0,groundsize}),
		glm::translate(df,{0.0,0.0,groundsize}),
		glm::translate(df,{groundsize,0.0,groundsize}),

		glm::translate(df,{-groundsize,0.0,0.0}),
		glm::translate(df,{groundsize,0.0,0.0}),

		glm::translate(df,{-groundsize,0.0,-groundsize}),
		glm::translate(df,{0.0,0.0,-groundsize}),
		glm::translate(df,{groundsize,0.0,-groundsize})
	};
	for (int i = 1; i < IM; i++) {
		for (std::vector<Obj*>::iterator o = MAP[i].begin(), e = MAP[i].end(); o != e; o++) {
			(*o)->M.at(0) *= trans[i];
		}
	}
	//groundsize;
}
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
	/*기본 배경*/
	Set_draw();
	
	Set_IM();
	//Make_IM();

	/*그리기 시작*/
	std::vector<Obj*> Alpha_objs;
	Alpha_objs.reserve(16);
	{
		glFrontFace(GL_CW);
		drawObj(world);
		glFrontFace(GL_CCW);
		drawObj(plane.obj);

		/*불투명*/
		drawObj(sun.obj);
		drawObj(moon.obj);
		drawObj(ground);
		for (int i = 0; i < buildingnum; i++) {
			drawObj(building[i]);
		}
		for (int i = 0; i < cloudnum; i++) {
			drawObj(cloud[i]);
		}
		for (std::vector<bullet_>::iterator i = bullet.begin(), e = bullet.end(); i != e; i++) {
			drawObj(i->obj);
		}
	}
	/*alpha*/
	/*투명 ALpha_objs 로 push 하면 정렬한 후 드로우 함*/
	{	
		/*
		최적화와 정확도 문제로 사용 중단.

		glm::mat4 PV = df * screen.proj_M() * camera.view_M();
		for (int i = 0; i < cloudnum; i++) {
			if (!cull_this_in_proj(cloud[i], PV)) {
				Alpha_objs.push_back(&cloud[i]);
			}
		}
		*/
	}
	/*그리기 끝*/
	Sort_Alpha_blending(Alpha_objs);
	Draw_Alpha_blending(Alpha_objs);
	
	glutSwapBuffers();
}

/*이벤트 함수*/
bool P_go, P_stop, P_YL, P_YR, P_RL, P_RR, P_PU, P_PD, timeStop, bl_rain, stealth, bl_shot;
bool up, down, left, right;
GLvoid Timer(int value) {
	constexpr GLfloat degree{ 5.0f };
	switch (value)
	{
	case 0: {
		if (timeStop == false) {
			constexpr GLfloat light_degree{ -1.0f };
			sun.pos = glm::rotate(df, glm::radians(light_degree), { 0.0,0.0,1.0 }) * glm::vec4{ sun.pos ,1.0 };
			sun.update();
			if (ground_floor < sun.pos.y) {
				GLfloat d = sun.pos.y / groundsize;
				LIGHT::ambient = d;
				LIGHT::ambientColor = { 1.0,d,1.0 };
				sun.on();
			}
			else sun.off();
			moon.pos = glm::rotate(df, glm::radians(light_degree), { 0.0,0.0,1.0 }) * glm::vec4{ moon.pos ,1.0 };
			moon.update();
			if (ground_floor < moon.pos.y) {
				LIGHT::ambient = 0.3f;
				LIGHT::ambientColor = { 0.2,0.2,1.0 };
				moon.on();
			}
			else moon.off();
		}
		
		if (stealth) {
			constexpr int tm{ 50 };
			constexpr int tm2{ 25 };
			static int time{ tm };
			if(tm2<time)plane.Stealth(true);
			else if(time%2) {
				plane.Stealth(true);
			}
			else {
				plane.Stealth(false);
			}
			time--;
			if (time < 0)stealth = false, time = tm;
		}
		
		
		glutTimerFunc(50, Timer, value);
		break;
	}
	case 1: {
		/**/
		if (bl_shot) {
			bullet.push_back(bullet_());;
			bullet_* a = &(bullet.back());
			LoadObj("sphere.obj", a->obj, "8/8/8");
			a->obj.M.resize(3, df);
			// 총알 머리 = 뱅기 머리.
			a->obj.M[2] = glm::scale(df, { 0.1,0.1,1.0 });
			// 총알 회전 == 뱅기 회전.
			a->obj.M[1] = plane.obj.M[1];
			a->obj.M[0] = plane.obj.M[0];
			a->obj.Set_Color({ 100.0f,100.0f,100.0f,1.0f });
			
			a->dir = -1.0f * camera.Dir();
			a->obj.M[0] *= glm::translate(df, a->dir);
			//update 에서 함 b->obj.M[0] = plane.obj.M[0] * glm::translate(df, { plane.nDir() * -3.0f });
		}
		std::vector<std::vector<bullet_>::iterator> trashcan;
		for (std::vector<bullet_>::iterator i = bullet.begin(), e = bullet.end(); i != e; i++) {
			i->obj.M[0] *= glm::translate(df, i->dir * (plane.maxspeed+10.0f));
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

		/**/
		if (up) {
			glm::mat4 R = glm::rotate(df, glm::radians(-degree/(FPS/6)), camera.Right());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (down) {
			glm::mat4 R = glm::rotate(df, glm::radians(degree/(FPS / 6)), camera.Right());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (right) {
			glm::mat4 R = glm::rotate(df, glm::radians(degree/(FPS / 6)), camera.Up());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (left) {
			glm::mat4 R = glm::rotate(df, glm::radians(-degree/(FPS / 6)), camera.Up());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}

		if (P_go) {
			plane.set_speed(0.08f);
		}
		if (P_stop) {
			plane.set_speed(-0.1f);
		}
		if (P_YL) {
			//ver 1
			//plane.Yaw(degree / (FPS / 6));
		}
		if (P_YR) {
			//ver 1
			//plane.Yaw(-degree / (FPS / 6));
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
		plane.update();
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

			break;
		}
		case GLUT_DOWN: {

			break;
		}
		default: { break; }
		}
		break;
	}
	case GLUT_RIGHT_BUTTON: {
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
GLvoid Motion(int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	
	glutPostRedisplay();
}
GLvoid MouseWheel(int button, int dir, int x, int y) {
	if (dir > 0)
	{
		plane.view_dist_add(-10.0f);
	}
	else
	{
		plane.view_dist_add(10.0f);
	}
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
		bl_rain = bl_rain ? false : true;
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
	case '0': {
		if (!stealth) {
			stealth = true;
		}
		break;
	}
	case '\t': {
		for (int i = 0; i < buildingnum; i++) {
			building[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize)*2) - groundsize,-40.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
		break;
	}
	case '1': {
		plane.HeadLightOnOff();
		break;
	}
	case '2': {
		plane.head_light.spot_theta = 2.5f;
		plane.head_light.col = glm::vec3(1.0f);
		break;
	}
	case '3': {
		plane.head_light.spot_theta = 1.f;
		plane.head_light.col = glm::vec3(2.0f);
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