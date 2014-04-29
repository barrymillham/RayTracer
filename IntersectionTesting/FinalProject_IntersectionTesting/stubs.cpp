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
	vec4 D(glm::normalize(v0), 0); // note: D = || P - E || = || v0 || (recall that v0 = P - E)
    mat4 tStarInv = tInv; // tInv with three elements zeroed out - a special form that we need to transform D
    tStarInv[3][0] = tStarInv[3][1] = tStarInv[3][2] = 0;
    D = tStarInv * D;

	// Transform p0 with tInv (we don't want the three elements zeroed out for this)
	vec4 p(p0, 1);
	p = tInv * p;
	// Since we're in model space, the sphere is simply a unit sphere centered at the origin.
	// Substituting our ray equation, R = p + tD, into the sphere's equation, we get a quadratic equation with:
	// a = 1; b = 2D . (p - (0,0,0)); c = || p - (0,0,0) ||^2 - 1

	// Solve for t:
	vec3 dTimes2 = v4Tov3(D);
	dTimes2 *= 2;
	double a = 1.0;
	double b = glm::dot(dTimes2, v4Tov3(p));
	double pLen = glm::length(v4Tov3(p));
	double c = pLen*pLen - 1;
	
	double discriminant = b*b - 4*a*c;
	if(discriminant == 0) // one real solution
	{
		double t = (-b)/(2*a);
		return t;
	}
	else if(discriminant > 0) // two real solutions
	{
		double t1 = (-b + sqrt(discriminant))/(2*a);
		double t2 = (-b - sqrt(discriminant))/(2*a);
		// If either value of t is positive, we want the smaller; otherwise, there's no intersection, and we should return -1
		if(t1 <= t2 && t1 >= 0)
			return t1;
		else if(t1 > t2 && t2 >= 0)
			return t2;
		else
			return -1;
	}
	else // no solutions, i.e. no intersection
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