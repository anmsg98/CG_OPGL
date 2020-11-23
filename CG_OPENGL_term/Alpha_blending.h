#pragma once
#include"Header.h"
#include"Obj.h"
#include"camera.h"

bool FarFromEYE(Obj* a, Obj* b);
GLvoid Sort_Alpha_blending(std::vector<Obj*>& all_obj);
GLvoid Draw_Alpha_blending(std::vector<Obj*>& all_obj);