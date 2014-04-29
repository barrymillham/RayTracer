#include "stubs.h"

using namespace glm;

double Test_RaySphereIntersect(const vec3& P0, const vec3& V0, const mat4& T) {
	mat4 tInv = glm::inverse(T);

	return raySphereIntersect(P0, V0, tInv);
}

double Test_RayPolyIntersect(const vec3& P0, const vec3& V0, const vec3& p1, const vec3& p2, const vec3& p3, const mat4& T) {
	mat4 tInv = glm::inverse(T);

	return rayTriangleIntersect(P0, V0, p1, p2, p3, tInv);
}

double Test_RayCubeIntersect(const vec3& P0, const vec3& V0, const mat4& T) {
	mat4 tInv = glm::inverse(T);

	return rayCubeIntersect(P0, V0, tInv);
}

double raySphereIntersect(const vec3 &p0, const vec3 &v0, const mat4 &tInv)
{

	
	return -1;
}

double rayTriangleIntersect(const vec3 &p0, const vec3 &v0, const vec3 &p1, const vec3 &p2, const vec3 &p3, const mat4 &tInv)
{


	return -1;
}

double rayCubeIntersect(const vec3 &p0, const vec3 &v0, const mat4 &tInv)
{


	return -1;
}