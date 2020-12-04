#pragma once
#include"Header.h"

#ifndef OBJ_H
#define OBJ_H
#endif // !OBJ_H

enum class COLOR_
{
	RED,
	GREEN,
	BLUE,
	YELLOW,
	count
};

//prefix
inline COLOR_& operator++(COLOR_& c) {
	typedef std::underlying_type_t<COLOR_> ut;
	auto constexpr cnt = (ut)COLOR_::count;
	c = COLOR_(((ut)c + 1) % cnt);
	return c;
}
//postfix
inline COLOR_ operator++(COLOR_& c, int) {
	auto const res = c;
	++c;
	return res;
}



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


GLvoid ChangeCol(Obj& o, COLOR_ c);
GLfloat* coll_box(Obj& obj);
void cout_coll_box(Obj& obj);
bool LoadObj(const GLchar objFile[], Obj& obj, const GLchar f_style[]);
GLvoid drawObj(Obj& o);