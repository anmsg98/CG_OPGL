#include"shader_sunkue.h"

GLuint shaderID;

GLuint worldLoc;
GLuint viewLoc;
GLuint projLoc;
GLuint viewPosLoc;
GLuint shineLoc;
GLuint ambientLoc;
GLuint ambientColorLoc;
GLuint specLoc;
GLuint texLoc;
GLuint use_texLoc;
GLuint light_numLoc;


//array
GLuint lightColLoc;
GLuint lightPosLoc;
GLuint use_spotLoc;
GLuint spot_cosLoc;
GLuint spot_out_cosLoc;
GLuint spot_dirLoc;


GLvoid init_uniform_Loc()
{
	worldLoc = glGetUniformLocation(shaderID, "worldMat");
	viewLoc = glGetUniformLocation(shaderID, "viewMat");
	projLoc = glGetUniformLocation(shaderID, "projMat");
	viewPosLoc = glGetUniformLocation(shaderID, "viewPos");
	shineLoc = glGetUniformLocation(shaderID, "shine");
	ambientLoc = glGetUniformLocation(shaderID, "ambientLight");
	specLoc = glGetUniformLocation(shaderID, "spec");
	texLoc = glGetUniformLocation(shaderID, "texture");
	use_texLoc = glGetUniformLocation(shaderID, "use_tex");
	light_numLoc = glGetUniformLocation(shaderID, "light_num");
	ambientColorLoc = glGetUniformLocation(shaderID, "ambientColor");
	
	//array
	use_spotLoc = glGetUniformLocation(shaderID, "use_spot");
	spot_cosLoc = glGetUniformLocation(shaderID, "spot_cos");
	spot_out_cosLoc = glGetUniformLocation(shaderID, "spot_out_cos");
	spot_dirLoc = glGetUniformLocation(shaderID, "spot_dir");
	lightColLoc = glGetUniformLocation(shaderID, "lightColor");
	lightPosLoc = glGetUniformLocation(shaderID, "lightPos");
}
GLchar* ReadSource(const GLchar file[])
{
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
		std::cerr << "ERROR: file read ����\n" << file << '\n';
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

	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		GLchar errorLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << '\n';
		return false;
	}
	return vertexShader;
}
GLuint make_fragmentShaders(const GLchar file[])
{
	GLchar* fragmentShaderSource;
	fragmentShaderSource = ReadSource(file);/*.glsl*/
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		GLchar errorLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << '\n';
		return false;
	}
	return fragmentShader;
}
GLuint make_shaderProgram(const GLuint vertexShader, const GLuint fragmentShader)
{
	GLuint ShaderProgramID = glCreateProgram(); //--- ���̴� ���α׷� �����
	glAttachShader(ShaderProgramID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(ShaderProgramID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(ShaderProgramID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ���α׷��� ��ũ�Ͽ� ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	GLint result{ 0 };
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		GLchar errorLog[512];
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << '/n';
		return false;
	}
	/*���� ��𼭳� �θ� �� �ִ�. ���Ⱑ �ƴϿ��� �ȴ�.*/
	glUseProgram(ShaderProgramID);	//--- ������� ���̴� ���α׷� ����ϱ�
									//--- ���� ���� ���̴����α׷� ���� �� �ְ�, Ư�� ���α׷��� ����Ϸ���
									//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
									//---** ����ϱ� ������ ȣ���� �� �ִ�.
	return ShaderProgramID;
}
GLuint InitShader()
{
	GLuint vertexShader = make_vertexShaders("shader/vertex.txt");
	GLuint fragmentShader = make_fragmentShaders("shader/fragment.txt");
	shaderID = make_shaderProgram(vertexShader, fragmentShader);
	return shaderID;
}
#include<chrono>
bool InitBuffer(Obj& obj)
{
	auto timer1{ std::chrono::high_resolution_clock::now() };
	//if (obj.objData.VAO != 0)glDeleteVertexArrays(1, &(obj.objData.VAO));
	GLuint abo;	// array buffer obj
	GLuint ebo;	// elements bo
	//--- Vertex Array Object ����
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
		std::cerr << "position �Ӽ� ���� ����" << '\n';
		return false;
	}
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
	//--- vertex color(frag)
	GLint colorAttribute = glGetAttribLocation(shaderID, "color");
	if (colorAttribute == -1) {
		std::cerr << "color �Ӽ� ���� ����" << '\n';
		return false;
	}
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, col));


	//--- normal
	GLint nomalAttribute = glGetAttribLocation(shaderID, "normal");
	if (nomalAttribute == -1) {
		std::cerr << "normal �Ӽ� ���� ����" << '\n';
		return false;
	}
	glEnableVertexAttribArray(nomalAttribute);
	glVertexAttribPointer(nomalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, nor));

	//--- texture coordinates
	GLint texCoordAttribute = glGetAttribLocation(shaderID, "texCoord");
	if (texCoordAttribute == -1) {
		std::cerr << "texCoord �Ӽ� ���� ����" << '\n';
		return false;
	}
	glEnableVertexAttribArray(texCoordAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, tex));

	
	glBindVertexArray(0);
	glDeleteBuffers(1, &abo);
	glDeleteBuffers(1, &ebo);

	auto timer2{ std::chrono::high_resolution_clock::now() };
	char title[512] = "";
	_itoa_s((timer2 - timer1).count(), title, 10);
	timer1 = timer2;
	glutSetWindowTitle(title);
	
	return true;
}