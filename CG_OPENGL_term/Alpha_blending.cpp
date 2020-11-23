#include"Alpha_blending.h"

bool FarFromEYE(Obj* a, Obj* b) {
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