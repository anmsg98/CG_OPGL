#include"screen.h"

SCREEN screen;


GLfloat SCREEN::aspect() { return width / height; }
glm::mat4 SCREEN::proj_M() { return glm::perspective(glm::radians(this->fovy), this->aspect(), this->n, this->f); }