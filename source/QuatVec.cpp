
#include "QuatVec.h"
#include <gtx/transform.hpp>

using glm::vec3;
using glm::fquat;
using glm::mat4;

QuatVec::QuatVec()
	: trans(), rot(0,0,0,1), mode(Type::TR){
}

QuatVec::QuatVec(vec3 t, fquat r, Type m)
	: trans(t), rot(glm::normalize(r)), mode(m) {
}

QuatVec::QuatVec(fquat r, vec3 t, Type m)
	: trans(t), rot(glm::normalize(r)), mode(m){
}

QuatVec QuatVec::operator * (const float s){
	return QuatVec(s*trans, s*rot, mode);
}

QuatVec QuatVec::operator + (const vec3& T){
	return QuatVec(T+trans, rot, mode);
}

QuatVec QuatVec::operator * (const fquat& R){
	return QuatVec(trans, R*rot, mode);
}

//I suspect this will cause MASSIVE problems down the road...
QuatVec QuatVec::operator * (const QuatVec& other){
	return QuatVec(trans + other.trans, rot*other.rot, mode);
}

QuatVec QuatVec::operator + (const QuatVec& other){
	return QuatVec(trans + other.trans, rot + other.rot, mode);
}

void QuatVec::operator *= (const float s){
	this->trans *= s;
	this->rot *= s;
}

QuatVec QuatVec::blend(const QuatVec& other, const float a){
	const float EPS = 0.001f;
	vec3 T = glm::mix(trans, other.trans, a);
	fquat c(other.rot);

	//cout << this->rot << "\t" << this->trans << "\n" << other.rot << "\t" << other.trans << endl;

	float cosTheta = glm::dot(rot, c);

	if (cosTheta < 0){
		c = -c;
		cosTheta = -cosTheta;
	}
	if (cosTheta >(1 - EPS)){//if the interpolation angle is very small, just lerp
		return QuatVec(T, fquat(
			glm::mix(rot.w, c.w, a),
			glm::mix(rot.x, c.x, a),
			glm::mix(rot.y, c.y, a),
			glm::mix(rot.z, c.z, a)
			));
	}

	// Essential Mathematics, page 467
	float angle = acos(cosTheta);
	return QuatVec(T, (rot*(float)sin(angle*(1 - a)) + c*(float)sin(a*angle)) / (float)sin(angle));
}

QuatVec QuatVec::inverse(){
	return QuatVec(-trans, glm::inverse(rot));
}

mat4 QuatVec::toMat4(){
	switch (mode){
	case Type::RT:
		return glm::mat4_cast(rot)*glm::translate(trans);
	case Type::TRT:
		return glm::translate(trans)*glm::mat4_cast(rot)*glm::translate(-trans);
	case Type::TR:
	default:
		return glm::translate(trans)*glm::mat4_cast(rot);
	}
}