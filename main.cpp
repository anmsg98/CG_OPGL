#include"Header.h"
#include"light.h"
#include"Obj.h"
#include"texture_sunkue.h"
#include"shader_sunkue.h"
#include"camera.h"
#include"PLANE.h"
#include"screen.h"
/*
	ambient : space 
	camera : 1 2 3
	speed w s
	rotate a d 8(up) 4(left) 5(down) 6(right) 
	stealth 0
	snow Q
	building tap
*/

/*Funcs*/
GLvoid drawObj(Obj& o);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid SpecialInput(int key, int x, int y);
GLvoid SpecialInput_up(int key, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid keyboard_up(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid MouseWheel(int button, int dir, int x, int y);
GLvoid Timer(int value);
GLvoid MakeShape();
GLvoid DefaultObj();
GLvoid print_message();
/*struct funcs*/


Obj coordinate, world, ground, building[buildingnum];
std::vector<Obj> rain;
LIGHT sun, moon;
std::vector<LIGHT> thunder, bullet;

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

	/*  */
	set_flip_texture(true);
	DefaultObj();
	MakeShape();

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

	glutTimerFunc(50, Timer, 0);

	glutMainLoop();

	print_message();
}
GLvoid print_message() {
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	std::cout <<
		"\
		light[space]\n\
		plane[w a s d 0 up(8) down(5) left(4) right(6)]\n\
		camera[1 2 3]\n\
		snow[q]\n\
		";
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
}
GLvoid DefaultObj() {
	/*world*/
	LoadObj("sphere.obj", world, "8/8/8");
	LoadTexture(world, "skydome.jpg", 2048, 1024, 3);
	world.M.push_back(glm::scale(df, screen.size_of_world*100.0f));
	world.Reverse_nor();

	/*light*/
	LoadObj("sphere.obj", sun.obj, "8/8/8");
	sun.obj.Set_Color({ 1.0, 0.2, 0.2, 1.0 });
	sun.pos = { 500.0, 250.0, 0.0 };
	sun.col = { 1.0,1.0,1.0 };
	sun.obj.M.push_back(glm::translate(df, sun.pos));
	sun.obj.M.push_back(glm::scale(df, { 0.8f,0.8f,0.8f }));
	sun.on();
	LoadObj("sphere.obj", moon.obj, "8/8/8");
	moon.obj.Set_Color({ 0.2, 0.2, 1.0, 1.0 });
	moon.pos = {-500.0,250.0,0.0};
	moon.col = { 1.0,1.0,1.0 };
	moon.obj.M.push_back(glm::translate(df, moon.pos));
	moon.obj.M.push_back(glm::scale(df, { 0.8f,0.8f,0.8f }));
	moon.on();

	/*coord*/
	{
		Vertex V;
		GLfloat sizex = screen.size_of_world[0] * 0.8f;
		GLfloat sizey = screen.size_of_world[1] * 0.8f;
		GLfloat sizez = screen.size_of_world[2] * 0.8f;
		constexpr int VofC{ 100 };
		constexpr int dd{ VofC - 1 };
		constexpr int iy = 2 * VofC;
		constexpr int iz = 3 * VofC;
		for (GLfloat i = 0; i < iz; i++) {
			if (i < VofC) {
				V.col = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
				V.pos = glm::vec3(-sizex + 2 * i * sizex / dd, 0.0f, 0.0f);
			}
			else if (i < iy) {
				V.col = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				V.pos = glm::vec3(0.0f, -sizey + 2 * (i - VofC) * sizey / dd, 0.0f);
			}
			else {
				V.col = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
				V.pos = glm::vec3(0.0f, 0.0f, -sizez + 2 * (i - iy) * sizez / dd);
			}
			coordinate.objData.vertices.push_back(V);
			coordinate.objData.verIndices.push_back(i);
		}
		coordinate.shape = GL_LINES;
		InitBuffer(coordinate);
	}
}
GLvoid MakeShape() {
	plane.init();
	{
		for (int i = 0; i < buildingnum; i++) {
			LoadObj("apartment.obj", building[i], "8/8/8");
			building[i].Set_Color({ 0.5f,0.5f,GLfloat(rand() % 10) / 10.0f,1.0f });
			building[i].M.resize(3, df);
			building[i].M.at(2) = glm::scale(df, glm::vec3(1.0f));
			building[i].M.at(1) = glm::rotate(df,glm::radians(GLfloat(rand()%360)), {0.0,1.0,0.0});
			building[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize)*2) - groundsize,-50.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
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

/*그리기 함수*/
GLvoid drawObj(Obj& o) {
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(o.world_M()));
	glUniform1i(shineLoc, o.objData.shine);
	glUniform1i(use_texLoc, o.use_texture);
	glBindVertexArray(o.objData.VAO);
	glPolygonMode(GL_FRONT, o.drawmode);
	if (o.use_texture) { glActiveTexture(GL_TEXTURE0), glBindTexture(GL_TEXTURE_2D, o.texture); }
	glDrawElements(o.shape, o.objData.verIndices.size(), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
bool FarFromEYE(Obj* a,Obj* b) {
	glm::vec3 A{ a->world_M() * glm::vec4(a->objData.vertices.at(0).pos,1.0f) };
	glm::vec3 B{ b->world_M() * glm::vec4(b->objData.vertices.at(0).pos,1.0f) };
	return glm::distance(B, camera.EYE) < glm::distance(A, camera.EYE);
}
GLvoid Sort_Alpha_blending(std::vector<Obj*>& all_obj) {
	sort(all_obj.begin(), all_obj.end(), FarFromEYE);
}
GLvoid Draw_Alpha_blending(std::vector<Obj*>& all_obj) {
	for (std::vector<Obj*>::iterator i{ all_obj.begin() }, e{ all_obj.end() }; i != e; i++) {
		drawObj(**i);
	}
}
GLvoid drawScene() {
	/*기본 배경*/
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.view_M()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(screen.proj_M()));
	//glUniform3f(lightPosLoc, sun.pos.x, sun.pos.y, sun.pos.z);
	//glUniform3f(lightColLoc, sun.col.x, sun.col.y, sun.col.z);
	LIGHT::init_light_buffer();
	glUniform3fv(lightPosLoc, LIGHT::light_num, LIGHT::lights_pos);
	glUniform3fv(lightColLoc, LIGHT::light_num, LIGHT::lights_col);
	glUniform3f(ambientColorLoc, LIGHT::ambientColor.r, LIGHT::ambientColor.g, LIGHT::ambientColor.b);
	glUniform3f(viewPosLoc, camera.EYE.x, camera.EYE.y, camera.EYE.z);
	glUniform1f(ambientLoc, LIGHT::ambient);
	glUniform3f(specLoc, sun.spec.r, sun.spec.g, sun.spec.b);
	glUniform1i(texLoc, 0);
	glUniform1i(light_numLoc, LIGHT::light_num);

	//std::cout << light_num;
	std::vector<Obj*> Alpha_objs;
	/*그리기 시작*/
	{
		glFrontFace(GL_CW);
		drawObj(world);
		glFrontFace(GL_CCW);
		drawObj(plane.obj);

		/*불투명*/
		drawObj(sun.obj);
		drawObj(moon.obj);
		drawObj(ground);
		for (std::vector<Obj>::iterator i{ rain.begin() }, e{ rain.end() }; i != e; i++) {
			drawObj(*i);
		}
		for (int i = 0; i < buildingnum; i++) {
			drawObj(building[i]);
		}
	}
	/*alpha*/
	{
		/*투명 ALpha_objs 로 push 하면 정렬한 후 드로우 함*/
	}
	/*그리기 끝*/
	Sort_Alpha_blending(Alpha_objs);
	Draw_Alpha_blending(Alpha_objs);
	glutSwapBuffers();
}

/*이벤트 함수*/
bool P_go, P_stop, P_YL, P_YR, P_RL, P_RR, P_PU, P_PD, bl_snow,stealth;
bool up, down, left, right;
GLvoid Timer(int value) {
	constexpr GLfloat degree{ 5.0f };
	switch (value)
	{
	case 0: {		
		bool s = false;
		if (up) {
			s = true;
			glm::mat4 R = glm::rotate(df, glm::radians(-degree), camera.Right());
			camera.EYE = glm::vec3(R * glm::vec4(camera.EYE, 1.0f));
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (down) {
			s = true;
			glm::mat4 R = glm::rotate(df, glm::radians(degree), camera.Right());
			camera.EYE = glm::vec3(R * glm::vec4(camera.EYE, 1.0f));
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (right) {
			s = true;
			glm::mat4 R = glm::rotate(df, glm::radians(degree), camera.Up());
			camera.EYE = glm::vec3(R * glm::vec4(camera.EYE, 1.0f));
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (left) {
			s = true;
			glm::mat4 R = glm::rotate(df, glm::radians(-degree), camera.Up());
			camera.EYE = glm::vec3(R * glm::vec4(camera.EYE, 1.0f));
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}

		plane.update(s);
		// P_go, P_stop, P_YL, P_YR, P_RL, P_RR, P_PU, P_PD

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

		if (P_go) {
			plane.set_speed(0.05f);
		}
		if (P_stop) {
			plane.set_speed(-0.1f);
		}
		if (P_YL) {
			plane.Yaw(degree/4);
		}
		if (P_YR) {
			plane.Yaw(-degree/4);
		}
		if (P_RL) {
			plane.Roll(degree);
		}
		if (P_RR) {
			plane.Roll(-degree);
		}
		if (P_PU) {
			plane.Pitch(degree/5);
		}
		if (P_PD) {
			plane.Pitch(-degree/5);
		}

		
			if (250 <= rain.size()) { rain.begin()->objData.DelObjData(); rain.erase(rain.begin()); }
			for (std::vector<Obj>::iterator i{ rain.begin() }, e{ rain.end() }; i != e; i++) {
				if ((i->world_M()* glm::vec4{ i->objData.vertices.at(0).pos, 1.0f }).y <= -45.0f) {
					i->M.at(2) = glm::scale(df, { 2.0,0.1,2.0 });
				}
				else i->M.at(0) = glm::translate(i->M.at(0), { 0.0,-1.0,0.0 });
			}
		
		glutTimerFunc(50, Timer, value);
		break;
	}
	case 241: {
		Obj o;
		LoadObj("sphere.obj", o, "8/8/8");
		o.Set_Color({ 1.0f,GLfloat(rand() % 10) / 10.0f,1.0f,1.0f });
		o.M.push_back(glm::translate(glm::mat4(1.0f), { GLfloat(rand() % 200) - 100.0f,GLfloat(rand() % 20 + 80),GLfloat(rand() % 200) - 100.0f }));
		o.M.push_back(glm::scale(df, {0.1,1.0,0.1}));
		o.M.resize(3, df);
		rain.push_back(o);

		if (bl_snow)glutTimerFunc(100, Timer, value);
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
	}
	else
	{
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
		plane.eye_mode = 0;
		break;
	}
	case '2': {
		plane.eye_mode = 1;
		break;
	}
	case '3': {
		plane.eye_mode = 2;
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
	case ' ': {
		if (LIGHT::ambient < 1.0f)LIGHT::ambient += 0.4f;
		else LIGHT::ambient = 0.0f;

		break;
	}
	case 'Q':
	case 'q': {
		bl_snow = bl_snow ? false : true;
		glutTimerFunc(50, Timer, 241);
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
	default:
		break;
	}
	glutPostRedisplay();
}
GLvoid Reshape(int w, int h) {
	screen.width = w, screen.height = h;
	glViewport(0, 0, screen.width, screen.height);
}