#pragma once


#ifndef OBJ_H
#include"Obj.h"
#endif // !OBJ_H

void set_flip_texture(bool set);
bool LoadTexture(Obj& obj, const char file[], GLsizei width, GLsizei height, int numOfChannel);