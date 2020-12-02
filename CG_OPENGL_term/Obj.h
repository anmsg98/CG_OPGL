#pragma once
#include"Header.h"

#ifndef OBJ_H
#define OBJ_H
#endif // !OBJ_H


typedef GLushort Index;
struct Vertex {
	glm::vec3 pos; // position
	glm::vec4 col{ 1.0f,1.0f,1.0f,1.0f }; // color
	glm::vec2 tex; // texCoord
	glm::vec3 nor; // normal
};
struct ObjData {
	GLuint VAO{ 0 };
	GLint shine{ 16 };
	std::vector<Vertex> vertices;
	std::vector<Index> verIndices;
	void DelObjData();
};
struct Obj {
	ObjData objData;
	GLint use_texture{ false };
	GLuint texture;
	GLenum drawmode{ GLU_FILL };
	GLenum shape{ GL_TRIANGLES };
	std::vector<glm::mat4> M;

	glm::mat4 world_M();
	void DelObj();
	void Set_Alpha(const GLfloat alpha);
	void Set_Color(const glm::vec4& color);
	void Reverse_nor();
};

bool LoadObj(const GLchar objFile[], Obj& obj, const GLchar f_style[]);
GLvoid drawObj(Obj& o);