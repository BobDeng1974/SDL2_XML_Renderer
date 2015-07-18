
#ifndef QUATVEC_H
#define QUATVEC_H

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm.hpp>
#include <gtc/quaternion.hpp>

class QuatVec{
	enum class Type{
		TR,
		RT,
		TRT
	};
public:
	glm::vec3 trans;
	glm::fquat rot;
	Type mode;

public:
	QuatVec();
	QuatVec(glm::vec3, glm::fquat, Type m = Type::TR);
	QuatVec(glm::fquat, glm::vec3, Type m = Type::RT);
	QuatVec operator * (const float s);
	QuatVec operator * (const QuatVec& other);
	QuatVec operator + (const QuatVec& other);
	QuatVec operator + (const glm::vec3& T);
	QuatVec operator * (const glm::fquat& R);
	char operator & (const QuatVec& other);
	void operator *= (const float s);
	QuatVec blend(const QuatVec& other, float a);
	QuatVec inverse();
	glm::mat4 toMat4();
};

#endif