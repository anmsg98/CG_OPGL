#include"texture_sunkue.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#endif // !STB_IMAGE_IMPLEMENTATION

constexpr char image_dir[]{ "res/image/" };

void set_flip_texture(bool set) {
	stbi_set_flip_vertically_on_load(set);
}
bool LoadTexture(Obj& obj, const char file[], GLsizei width, GLsizei height, int numOfChannel)
{

	//width,height = 2^n
	//obj.Set_Color(glm::vec4(1.0f)); // --회색조
	char F[256];
	strncpy_s(F, image_dir, sizeof(F));
	strncat_s(F, file, sizeof(F));
	GLuint texture;
	glGenTextures(1, &texture); //--- 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, texture); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); //GL_REPEAT //GL_CLAMP // GL_MIRRORED_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST // GL_LINEAR
	stbi_uc* data = stbi_load(F, &width, &height, &numOfChannel, 0); //--- 텍스처로 사용할 비트맵 이미지 로드하기
	if (data == nullptr) {
		std::cerr << "texture image error : " << file;
		return false;
	}
	GLenum format;
	switch (numOfChannel)
	{
	case 1: {
		format = GL_RED;
		break;
	}
	case 2: {
		format = GL_RG;
		break;
	}
	case 3: {
		format = GL_RGB;
		break;
	}
	case 4: {
		format = GL_RGBA;
		break;
	}
	default: {
		std::cerr << "image format error : " << file << '\n';
		return false;
		break;
	}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, numOfChannel, width, height, 0, format, GL_UNSIGNED_BYTE, data); //---텍스처 이미지 정의
	obj.texture = texture;
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	obj.use_texture = true;
	return true;
}