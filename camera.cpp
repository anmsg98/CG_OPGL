#include"camera.h"

CAMERA camera;

glm::vec3 CAMERA::Dir() { return glm::normalize(this->EYE - this->AT); }
glm::vec3 CAMERA::Right() { return glm::normalize(glm::cross(this->UP, this->Dir())); }
glm::vec3 CAMERA::Up() { return glm::normalize(glm::cross(this->Dir(), this->Right())); }
glm::mat4 CAMERA::view_M() { return glm::lookAt(this->EYE, this->AT, this->UP); }
