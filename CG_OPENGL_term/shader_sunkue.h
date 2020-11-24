#pragma once
#include"Header.h"
#ifndef OBJ_H
#include"Obj.h"
#endif // !OBJ_H

extern GLuint shaderID;

extern GLuint worldLoc;
extern GLuint viewLoc;
extern GLuint projLoc;
extern GLuint viewPosLoc;
extern GLuint shineLoc;
extern GLuint ambientLoc;
extern GLuint ambientColorLoc;
extern GLuint specLoc;
extern GLuint texLoc;
extern GLuint use_texLoc;
extern GLuint light_numLoc;

//array
extern GLuint lightColLoc;
extern GLuint lightPosLoc;
extern GLuint use_spotLoc;
extern GLuint spot_cosLoc;
extern GLuint spot_out_cosLoc;
extern GLuint spot_dirLoc;


GLvoid init_uniform_Loc();
GLchar* ReadSource(const GLchar file[]);
GLuint make_vertexShaders(const GLchar file[]);
GLuint make_fragmentShaders(const GLchar file[]);
GLuint make_shaderProgram(const GLuint vertexShader, const GLuint fragmentShader);
GLuint InitShader();
bool InitBuffer(Obj& obj);