#include"Obj.h"


void ObjData::DelObjData() { glDeleteVertexArrays(1, &this->VAO); vertices.clear(); verIndices.clear(); }


	glm::mat4 Obj::world_M() {
		glm::mat4 WM(1.0f);
		std::for_each(this->M.begin(), this->M.end(), [&WM](glm::mat4& m) { WM *= m; });
		//M0*M1*M2 TRS
		return WM;

	}
	void Obj::DelObj() { if (use_texture)glDeleteTextures(1, &this->texture), use_texture = false; objData.DelObjData(); }
	void Obj::Reverse_nor() {
		for (std::vector<Vertex>::iterator i{ this->objData.vertices.begin() }, e{ this->objData.vertices.end() }; i != e; i++) {
			i->nor = -i->nor;
		}
	}


