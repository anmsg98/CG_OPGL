#define STB_IMAGE_IMPLEMENTATION
#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
#include<string>
#include<time.h>
#include<gl/glew.h>
#include<algorithm>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/glm/glm.hpp>
#include<gl/glm/glm/ext.hpp>
#include<gl/glm/glm/gtc/matrix_transform.hpp>
#include"stb_image.h"
/*
	light : space 
	camera : 1 2 3
	speed w s
	rotate a d 8(up) 4(left) 5(down) 6(right) 
	stealth 0
	snow Q
	building tap
*/
constexpr glm::mat4 df(1.0f);
constexpr float ground_floor{-60.0f};
/* uniform */

GLuint worldLoc;
GLuint viewLoc;
GLuint projLoc;
GLuint lightColLoc;
GLuint lightPosLoc;
GLuint viewPosLoc;
GLuint shineLoc;
GLuint ambientLoc;
GLuint specLoc;
GLuint texLoc;
GLuint use_texLoc;

/*  */
GLuint shaderID;

/* structs */
typedef GLushort Index;
struct Vertex {
	glm::vec3 pos; // position
	glm::vec4 col{ 1.0f,1.0f,1.0f,1.0f }; // color
	glm::vec2 tex; // texCoord
	glm::vec3 nor; // normal
};
struct ObjData {
	GLuint VAO{0};
	GLint shine{ 16 };
	std::vector<Vertex> vertices;
	std::vector<Index> verIndices;
	void DelObjData() { glDeleteVertexArrays(1, &this->VAO); vertices.clear(); verIndices.clear(); }
};
struct Obj {
	ObjData objData;
	GLint use_texture{ false };
	GLuint texture;
	GLenum drawmode{ GLU_FILL };
	GLenum shape{ GL_TRIANGLES };
	std::vector<glm::mat4> M;
	glm::mat4 world_M() {
		glm::mat4 WM(1.0f);
		std::for_each(this->M.begin(), this->M.end(), [&WM](glm::mat4& m) { WM *= m; });
		//M0*M1*M2 TRS
		return WM;

	}
	void DelObj() { if (use_texture)glDeleteTextures(1, &this->texture), use_texture = false; objData.DelObjData(); }
	void Set_Alpha(const GLfloat alpha);
	void Set_Color(const glm::vec4& color);
	void Reverse_nor() {
		for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
			i->nor = -i->nor;
		}
	}
};
struct CAMERA {
	glm::vec3 EYE{ 0.0f,0.0f,1000.0f };
	glm::vec3 AT{ 0.0f,0.0f,0.0f };
	glm::vec3 UP{ 0.0f,1.0f,0.0f };
	glm::vec3 Dir() { return glm::normalize(this->EYE - this->AT); }
	glm::vec3 Right() { return glm::normalize(glm::cross(this->UP, this->Dir())); }
	glm::vec3 Up() { return glm::normalize(glm::cross(this->Dir(), this->Right())); }
	glm::mat4 view_M() {
		return glm::lookAt(this->EYE, this->AT, this->UP);
	}
}camera;
struct SCREEN {
	GLfloat fovy{ 30.0f }; //glm::Radians(fovy)
	GLsizei width{ 800 };	//W/H
	GLsizei height{ 600 };	//W/H
	GLfloat n{ 0.1f };
	GLfloat f{ 200000.0f };
	glm::vec3 size_of_world{ 100.0f,100.0f,100.0f };//[-a:a]
	GLfloat aspect() { return width / height; }
	glm::mat4 proj_M() { return glm::perspective(glm::radians(this->fovy), this->aspect(), this->n, this->f); }
}screen;
struct LIGHT {
	Obj obj;
	GLfloat ambient{ 0.7f };
	glm::vec3 spec{ 0.0f,0.0f,0.0f };
	glm::vec3 pos{ 500.0,250.0,0.0 };
	glm::vec3 col{ 1.0,1.0,1.0 };
}light;



/*Funcs*/
GLchar* ReadSource(const GLchar file[]);
GLuint make_vertexShaders(const GLchar file[]);
GLuint make_fragmentShaders(const GLchar file[]);
GLuint make_shaderProgram(GLuint vertexShader, GLuint fragmentShader);
GLuint InitShader(GLvoid);
bool InitBuffer(Obj& obj);
void set_flip_texture(bool set);
bool LoadTexture(Obj& obj, const char file[], GLsizei width, GLsizei height, int numOfChannel);
bool LoadObj(const GLchar objFile[], Obj& obj, const GLchar f_style[] = "8");	//	f = "8/"
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
void Obj::Set_Alpha(const GLfloat alpha) {
	for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
		i->col.a = alpha;
	}
	InitBuffer(*this);
}
void Obj::Set_Color(const glm::vec4& color) {
	for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
		i->col = color;
	}
	InitBuffer(*this);
}


/*user data*/
struct PLANE {
	Obj obj;
	glm::vec3 pos{ 0.0,0.0,0.0 };
	glm::vec3 head{ 0.0,0.0,-1.0 };
	glm::vec3 tail{ 0.0,0.0,1.0 };
	glm::vec3 up{ 0.0,1.0,0.0 };

	size_t eye_mode = 0;
	int mode{ 0 };
	GLfloat speed{ 0.0f };
	GLfloat maxspeed{ 20.0f };
	void init() {
		// 0 Trans	1 Yaw	2 Pitch	3 Roll	4 Size
		this->obj.M.resize(3, df);
		change_obj(mode);
		//this->obj.M.at(2) = glm::scale(df, glm::vec3(0.06));
		//this->obj.M.at(1) = glm::rotate(df, glm::radians(180.0f), { 0.0,1.0,0.0 });
		this->setPos();
		this->default_color();
	}

	void change_obj(int mode) {
		switch(mode) {
		case 0: {
			obj.DelObj();
			LoadObj("airplane.txt", this->obj, "8/8/8");
			this->obj.M.at(1) = glm::rotate(obj.M.at(1), glm::radians(180.0f), { 0.0,1.0,0.0 });
			this->obj.M.at(2) = glm::scale(df, glm::vec3(0.06));
			this->default_color();
			break;
		}
		case 1: {
			obj.DelObj();
			LoadObj("Plane.txt", this->obj, "8/8");
			this->obj.M.at(1) = glm::rotate(obj.M.at(1), glm::radians(180.0f), { 0.0,1.0,0.0 });
			this->obj.M.at(2) = glm::scale(df, glm::vec3(0.02));
			this->default_color();
			break;
		}
		}
	}

	void default_color() {
		this->obj.Set_Color({ 0.5,0.8,0.2,1.0 });
	}
	void Stealth(bool on) {
		if (on)obj.Set_Color({ 0.5,0.0,1.0,0.5 });
		else this->default_color();
	}
	void update_coor(glm::mat4& m) {
		head = glm::normalize(m * glm::vec4(head,1.0f));
		tail = glm::normalize(m * glm::vec4(tail,1.0f));
		up = glm::normalize(m * glm::vec4(up,1.0f));
	}

	glm::vec3 nDir() { return glm::normalize(this->tail - this->head); }
	glm::vec3 Right() { return glm::normalize(glm::cross(this->up, this->nDir())); }
	glm::vec3 Up() { return glm::normalize(glm::cross(this->nDir(), this->Right())); }

	glm::mat4 yaw_(GLfloat degree) {
		return glm::rotate(df, glm::radians(degree), Up());
	}
	glm::mat4 pitch_(GLfloat degree) {
		return glm::rotate(df, glm::radians(degree), Right());
	}
	glm::mat4 roll_(GLfloat degree) {
		return glm::rotate(df, glm::radians(degree), nDir());
	}

	void Yaw(GLfloat degree) {
		glm::mat4 m = yaw_(degree);
		obj.M.at(1) = m*obj.M.at(1);
		update_coor(m);
	}
	void Pitch(GLfloat degree) {
		glm::mat4 m = pitch_(degree);
		obj.M.at(1) =m*obj.M.at(1);
		update_coor(m);
	}
	void Roll(GLfloat degree) {
		glm::mat4 m = roll_(degree);
		obj.M.at(1) = m*obj.M.at(1);
		update_coor(m);
	}

	void go() {
		glm::mat4 m = glm::translate(df, nDir() * -speed);
		this->pos = m * glm::vec4{ this->pos,1.0f };
		check_area();
	}

	void check_area() {
		/*바닥 체크*/
		if (this->pos.y < ground_floor+15.0f) {
			this->obj.Set_Color({ 1.0,0.2,0.2,1.0 });
			this->pos.y = ground_floor+ 15.0f;
		}
	}

	void set_speed(GLfloat d) {
		speed += d;
		if (speed < 0.0f)speed = 0.0f;
		if (maxspeed < speed)speed = maxspeed;
	}

	void setPos() {
		this->obj.M.at(0) = glm::translate(df, pos);
	}

	void view() {
		glm::mat4 m;
		switch (eye_mode)
		{
		case 0: {m = glm::translate(df, nDir() * 100.0f); break; }
		case 1: {m= glm::translate(df, nDir()*400.0f); break;}
		case 2: {m = glm::translate(df, Right() * 5.0f)* glm::translate(df, nDir() * 10.0f); break; }
		}
		camera.AT =glm::translate(df, nDir() * -15.0f)* glm::vec4(this->pos, 1.0f);
		camera.EYE = m * glm::vec4{ this->pos ,1.0f };
		if (camera.EYE.y < ground_floor+20.0f)camera.EYE.y = ground_floor+ 20.0f;
	}

	void update(bool s) {	
		if(!s)this->view();
		this->go();
		this->setPos();
	}
}plane;

constexpr int buildingnum{ 50 };
constexpr GLfloat groundsize{ 1000.0f };
Obj coordinate, world, ground, building[buildingnum];
std::vector<Obj> snow;

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
	worldLoc = glGetUniformLocation(shaderID, "worldMat");
	viewLoc = glGetUniformLocation(shaderID, "viewMat");
	projLoc = glGetUniformLocation(shaderID, "projMat");
	lightColLoc = glGetUniformLocation(shaderID, "lightColor");
	lightPosLoc = glGetUniformLocation(shaderID, "lightPos");
	viewPosLoc = glGetUniformLocation(shaderID, "viewPos");
	shineLoc = glGetUniformLocation(shaderID, "shine");
	ambientLoc = glGetUniformLocation(shaderID, "ambientLight");
	specLoc = glGetUniformLocation(shaderID, "spec");
	texLoc = glGetUniformLocation(shaderID, "texture");
	use_texLoc = glGetUniformLocation(shaderID, "use_tex");

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
	world.M.push_back(glm::scale(df, screen.size_of_world));
	world.Reverse_nor();

	/*light*/
	LoadObj("sphere.obj", light.obj, "8/8/8");
	light.obj.Set_Color({ 1.0, 0.5, 0.5, 1.0 });
	light.obj.M.push_back(glm::translate(df, light.pos));
	light.obj.M.push_back(glm::scale(df, { 0.8f,0.8f,0.8f }));
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
			LoadObj("cube.txt", building[i], "8/8/8");
			building[i].Set_Color({ 1.0f,1.0f,GLfloat(rand() % 10) / 10.0f,1.0f });
			building[i].M.resize(2, df);
			building[i].M.at(1) = glm::scale(df, { 6.0f,50.0f,6.0f });
			building[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize)*2) - groundsize,-40.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
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

/*셰이더 함수*/
GLchar* ReadSource(const GLchar file[]) {
	std::ifstream in(file, std::ios::in);
	GLchar* s;
	if (in.is_open()) {
		in.seekg(0, std::ios::end);

		int size = in.tellg();
		s = new GLchar[size + 1]{ '\0' };
		in.seekg(0, std::ios::beg);
		in.read(&s[0], size);
	}
	else {
		std::cerr << "ERROR: file read 실패\n" << file << '\n';
		return nullptr;
	}
	//std::cout << "\n========\n"<<file<<'\n' << s << "\n==========\n";
	in.close();
	return s;
}
GLuint make_vertexShaders(const GLchar file[])
{
	GLchar* vertexShaderSource;
	vertexShaderSource = ReadSource(file);/*.glsl*/

	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		GLchar errorLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << '\n';
		return false;
	}
	return vertexShader;
}
GLuint make_fragmentShaders(const GLchar file[])
{
	GLchar* fragmentShaderSource;
	fragmentShaderSource = ReadSource(file);/*.glsl*/
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		GLchar errorLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << '\n';
		return false;
	}
	return fragmentShader;
}
GLuint make_shaderProgram(const GLuint vertexShader, const GLuint fragmentShader)
{
	GLuint ShaderProgramID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(ShaderProgramID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(ShaderProgramID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(ShaderProgramID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 프로그램에 링크하여 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	GLint result{ 0 };
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		GLchar errorLog[512];
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << '/n';
		return false;
	}
	/*언제 어디서나 부를 수 있다. 여기가 아니여도 된다.*/
	glUseProgram(ShaderProgramID);	//--- 만들어진 세이더 프로그램 사용하기
									//--- 여러 개의 세이더프로그램 만들 수 있고, 특정 프로그램을 사용하려면
									//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
									//---** 사용하기 직전에 호출할 수 있다.
	return ShaderProgramID;
}
GLuint InitShader() {
	GLuint vertexShader = make_vertexShaders("shader/vertex.txt");
	GLuint fragmentShader = make_fragmentShaders("shader/fragment.txt");
	shaderID = make_shaderProgram(vertexShader, fragmentShader);
	return shaderID;
}
bool InitBuffer(Obj& obj)
{
	if (obj.objData.VAO != 0)glDeleteVertexArrays(1, &(obj.objData.VAO));
	GLuint abo;	// array buffer obj
	GLuint ebo;	// elements bo
	//--- Vertex Array Object 생성
	glGenVertexArrays(1, &(obj.objData.VAO));
	glBindVertexArray(obj.objData.VAO);
	/*make abo and load data*/
	glGenBuffers(1, &abo);
	glBindBuffer(GL_ARRAY_BUFFER, abo);
	glBufferData(GL_ARRAY_BUFFER, obj.objData.vertices.size() * sizeof(Vertex), obj.objData.vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.objData.verIndices.size() * sizeof(Index), obj.objData.verIndices.data(), GL_STATIC_DRAW);

	//--- position(vertex)
	GLint positionAttribute = glGetAttribLocation(shaderID, "position");
	if (positionAttribute == -1) {
		std::cerr << "position 속성 설정 실패" << '\n';
		return false;
	}
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
	//--- vertex color(frag)
	GLint colorAttribute = glGetAttribLocation(shaderID, "color");
	if (colorAttribute == -1) {
		std::cerr << "color 속성 설정 실패" << '\n';
		return false;
	}
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, col));


	//--- normal
	GLint nomalAttribute = glGetAttribLocation(shaderID, "normal");
	if (nomalAttribute == -1) {
		std::cerr << "normal 속성 설정 실패" << '\n';
		return false;
	}
	glEnableVertexAttribArray(nomalAttribute);
	glVertexAttribPointer(nomalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, nor));

	//--- texture coordinates
	GLint texCoordAttribute = glGetAttribLocation(shaderID, "texCoord");
	if (texCoordAttribute == -1) {
		std::cerr << "texCoord 속성 설정 실패" << '\n';
		return false;
	}
	glEnableVertexAttribArray(texCoordAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, tex));

	glBindVertexArray(0);
	glDeleteBuffers(1, &abo);
	glDeleteBuffers(1, &ebo);
	return true;
}
/*데이터 로드 함수*/
void set_flip_texture(bool set) {
	stbi_set_flip_vertically_on_load(set);
}
bool LoadTexture(Obj& obj, const char file[], GLsizei width, GLsizei height, int numOfChannel)
{

	//width,height = 2^n
	obj.Set_Color(glm::vec4(1.0f)); // --회색조
	char F[256];
	strncpy_s(F, "res/", sizeof(F));
	strncat_s(F, file, sizeof(F));
	GLuint texture;
	glGenTextures(1, &texture); //--- 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, texture); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); //GL_REPEAT //GL_CLAMP // GL_MIRRORED_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST // GL_LINEAR
	stbi_uc* data = stbi_load(F, &width, &height, &numOfChannel, 0); //--- 텍스처로 사용할 비트맵 이미지 로드하기
	if (data == nullptr) {
		std::cerr << "texture image error : " << file;
	}
	GLenum format;
	switch (numOfChannel)
	{
	case 1: {
		format = GL_RED;
		break;
	}
	case 2: {
		format = GL_RG;
		break;
	}
	case 3: {
		format = GL_RGB;
		break;
	}
	case 4: {
		format = GL_RGBA;
		break;
	}
	default: {
		std::cerr << "image format error : " << file << '\n';
		return false;
		break;
	}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, numOfChannel, width, height, 0, format, GL_UNSIGNED_BYTE, data); //---텍스처 이미지 정의
	obj.texture = texture;
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	obj.use_texture = true;
	return true;
}
bool LoadObj(const GLchar objFile[], Obj& obj, const GLchar f_style[]) {
	std::string OBJ{ "obj/" };
	std::ifstream in(OBJ + objFile, std::ios::in);
	if (!in.is_open()) {
		std::cerr << "ERROR: Objfile read 실패\n" << objFile << '\n';
		return false;
	}

	char line[512]{ '\0' };
	Vertex V;
	std::vector<glm::vec3> P(1);
	std::vector<glm::vec2> T(1);
	std::vector<glm::vec3> N(1);

	glm::vec3 tv3;
	glm::vec2 tv2;

	while (!in.eof()) {
		in.getline(line, 512);
		if (strncmp(line, "v ", 2) == 0) {
			sscanf_s(&line[2], "%f %f %f", &tv3.x, &tv3.y, &tv3.z);
			P.push_back(tv3);
		}
		else if (strncmp(line, "vt ", 3) == 0) {
			sscanf_s(&line[3], "%f %f", &tv2.x, &tv2.y);
			T.push_back(tv2);
		}
		else if (strncmp(line, "vn ", 3) == 0) {
			sscanf_s(&line[3], "%f %f %f", &tv3.x, &tv3.y, &tv3.z);
			N.push_back(tv3);
		}
		else if (strncmp(line, "f ", 2) == 0) {
			/*
			char* end = line + sizeof(line) / sizeof(line[0]);
			int num = std::count_if(line, end, [](char c) {return c == '/'; });
			switch (num)
			{
			default:
				break;
			}*/
			unsigned int verI[3], texI[3], norI[3];
			bool vc{ false }, tc{ false }, nc{ false };
			if (f_style == "8") {
				vc = true;
				sscanf_s(&line[2], "%d %d %d", &verI[0], &verI[1], &verI[2]);
			}
			else if (f_style == "8//") {
				vc = true;
				sscanf_s(&line[2], "%d// %d// %d//", &verI[0], &verI[1], &verI[2]);
			}
			else if (f_style == "8/8/") {
				vc = true, tc = true;
				sscanf_s(&line[2], "%d/%d/ %d/%d/ %d/%d/", &verI[0], &texI[0],
					&verI[1], &texI[1], &verI[2], &texI[2]);
			}
			else if (f_style == "8//8") {
				vc = true, nc = true;
				sscanf_s(&line[2], "%d//%d %d//%d %d//%d", &verI[0], &norI[0],
					&verI[1], &norI[1], &verI[2], &norI[2]);
			}
			else if (f_style == "8/8/8") {
				vc = true, tc = true, nc = true;
				sscanf_s(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d", &verI[0], &texI[0], &norI[0],
					&verI[1], &texI[1], &norI[1], &verI[2], &texI[2], &norI[2]);
			}
			else if (f_style == "8/") {
				vc = true;
				sscanf_s(&line[2], "%d/ %d/ %d/", &verI[0], &verI[1], &verI[2]);
			}
			else if (f_style == "8/8") {
				vc = true, tc = true;
				sscanf_s(&line[2], "%d/%d %d/%d %d/%d", &verI[0], &texI[0],
					&verI[1], &texI[1], &verI[2], &texI[2]);
			}
			else return false;

			for (int i = 0; i < 3; i++) {
				if (vc) {
					V.pos = P[verI[i]];
					V.col = { rand() % 2 * 1.0f,rand() % 2 * 1.0f,rand() % 2 * 1.0f, 1.0f };
				}
				if (tc)V.tex = T[texI[i]];
				if (nc)V.nor = N[norI[i]];
				obj.objData.vertices.push_back(V);
			}
		}
	}
	obj.objData.verIndices.resize(obj.objData.vertices.size());
	std::generate(obj.objData.verIndices.begin(), obj.objData.verIndices.end(),
		[n = 0]() mutable { return n++; });
	in.close();
	InitBuffer(obj);
	return true;
}
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
	glUniform3f(lightPosLoc, light.pos.x, light.pos.y, light.pos.z);
	glUniform3f(lightColLoc, light.col.x, light.col.y, light.col.z);
	glUniform3f(viewPosLoc, camera.EYE.x, camera.EYE.y, camera.EYE.z);
	glUniform1f(ambientLoc, light.ambient);
	glUniform3f(specLoc, light.spec.r, light.spec.g, light.spec.b);
	glUniform1i(texLoc, 0);

	std::vector<Obj*> Alpha_objs;
	/*그리기 시작*/
	{
		glFrontFace(GL_CW);
		drawObj(world);
		glFrontFace(GL_CCW);
		drawObj(plane.obj);

		/*불투명*/
		drawObj(light.obj);
		drawObj(ground);
		for (std::vector<Obj>::iterator i{ snow.begin() }, e{ snow.end() }; i != e; i++) {
			drawObj(*i);
		}
	}
	/*alpha*/
	{
		/*투명 ALpha_objs 로 push 하면 정렬한 후 드로우 함*/
		for (int i = 0; i < buildingnum; i++) {
			Alpha_objs.push_back(&building[i]);
		}
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
			plane.set_speed(0.005f);
		}
		if (P_stop) {
			plane.set_speed(-0.01f);
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

		
			if (250 <= snow.size()) { snow.begin()->objData.DelObjData(); snow.erase(snow.begin()); }
			for (std::vector<Obj>::iterator i{ snow.begin() }, e{ snow.end() }; i != e; i++) {
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
		o.M.push_back(glm::scale(df, glm::vec3(0.2f)));
		o.M.resize(3, df);
		snow.push_back(o);

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
	case 'z': {
		plane.mode = plane.mode == 0 ? 1 : 0;
		plane.change_obj(plane.mode);
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
	case '5': {
		P_PD = true;
		break;
	}
	case '8': {
		P_PU = true;
		break;
	}	
	case ' ': {
		if (light.ambient < 1.0f)light.ambient += 0.4f;
		else light.ambient = 0.0f;

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
	case '5': {
		P_PD = false;
		break;
	}
	case '8': {
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