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
	vec4 D(glm::normalize(v0), 0); // note: D = || P - E || = || v0 || (recall that v0 = P - E)
	mat4 tStarInv = tInv; // tInv with three elements zeroed out - a special form that we need to transform D
	tStarInv[3][0] = tStarInv[3][1] = tStarInv[3][2] = 0;
	D *= tStarInv;

	//put triangle and ray in world space using the non-special transformation (tInv)
	vec4 pos(p0,1);
	pos *= tInv;
	vec4 point1(p1,1);
	point1 *= tInv;
	vec4 point2(p2,1);
	point2 *= tInv;
	vec4 point3(p3,1);
	point3 *= tInv;


	//RAY-PLANE INTERSECTION 
	//will find intersection point on plane, R
	vec4 R; //intersection point
	vec4 normal; //normal for triangle
	int t; //t in the ray equation

	//find normal of triangle
	normal = glm::cross(point1.xyz, point2);

	//find t
	//make sure the denominator isn't zero:
	int denom = 


	return -1;
}

double rayCubeIntersect(const vec3 &p0, const vec3 &v0, const mat4 &tInv)
{


	return -1;
}