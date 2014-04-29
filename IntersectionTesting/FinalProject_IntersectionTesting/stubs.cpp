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
	D = tStarInv * D;

	//put triangle in world space using the non-special transformation (tInv)
	vec4 pos(p0,1);
	pos = tInv * pos;
	vec4 point1(p1,1);
	//point1 = tInv * point1;
	vec4 point2(p2,1);
	//point2 = tInv * point2;
	vec4 point3(p3,1);
	//point3 = tInv * point3;


	//RAY-PLANE INTERSECTION 
	//will find intersection point on plane, R
	vec4 R; //intersection point
	vec3 normal; //normal for triangle
	float t; //t in the ray equation

	//find normal of triangle
	normal = glm::cross((v4Tov3(point1) - v4Tov3(point2)), (v4Tov3(point3) - v4Tov3(point2)));
	glm::normalize(normal);

	//find t
	//make sure the denominator isn't zero:
	float denom = glm::dot(normal, v4Tov3(D));
	if(denom == 0)
		return -1;

	float numerator = glm::dot(normal, v4Tov3(point1 - pos));

	t = numerator/denom;
	if(t<0) return -1;

	//now put t in ray equation to find intersection point R:
	D *= t;
	R = pos + D;

	//NOW TEST TO SEE IF POINT IS INSIDE TRIANGLE
	float s = area(v4Tov3(point1), v4Tov3(point2), v4Tov3(point3)); 
	float s1 = area(v4Tov3(R), v4Tov3(point2), v4Tov3(point3)) / s; 
	//if(s1 > 1 || s1 < 0) return -1; //we know it's outside the circle
	float s2 = area(v4Tov3(R), v4Tov3(point3), v4Tov3(point1)) / s; 
	//if(s2 > 1 || s2 < 0) return -1; //we know it's outside the circle
	float s3 = area(v4Tov3(R), v4Tov3(point1), v4Tov3(point2)) / s; 
	//if(s3 > 1 || s3 < 0) return -1; //we know it's outside the circle

	if(epsilonEquals(1, s1+s2+s3)) return t;

	return -1;
}

bool epsilonEquals(float n, float m) 
{
	if(abs(m-n) < 1e-3) return true;
	return false;
}

float area(vec3 p1, vec3 p2, vec3 p3)
{
	//construct matrices to be used to find determinants
	mat3 m1(p1.y, p1.z, 1, p2.y, p2.z, 1, p3.y, p3.z, 1);
	mat3 m2(p1.z, p1.x, 1, p2.z, p2.x, 1, p3.z, p3.x, 1);
	mat3 m3(p1.x, p1.y, 1, p2.x, p2.y, 1, p3.x, p3.y, 1);

	return .5*sqrt(glm::determinant(m1)*glm::determinant(m1) + glm::determinant(m2)*glm::determinant(m2) + glm::determinant(m3)*glm::determinant(m3));
}

double rayCubeIntersect(const vec3 &p0, const vec3 &v0, const mat4 &tInv)
{
	vec4 D(glm::normalize(v0), 0); // note: D = || P - E || = || v0 || (recall that v0 = P - E)
    mat4 tStarInv = tInv; // tInv with three elements zeroed out - a special form that we need to transform D
    tStarInv[3][0] = tStarInv[3][1] = tStarInv[3][2] = 0;
    D = tStarInv * D;

	//Transform p0 with tInv 
	vec4 p(p0, 1);
	p = tInv * p;

	//bounds are {-0.5, -0.5, -0.5} -> {0.5, 0.5, 0.5} since this is all in local space
	float T1x = (-0.5 - p.x) / D.x;
	float T2x = ( 0.5 - p.x) / D.x;
	float T1y = (-0.5 - p.y) / D.y;
	float T2y = ( 0.5 - p.y) / D.y;
	float T1z = (-0.5 - p.z) / D.z;
	float T2z = ( 0.5 - p.z) / D.z;

	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	
	
	/*	The next three defined floats are to avoid the case where the direction components would be -0.0
		This is only a small part of the time, but sometimes that can happen through various operations,
		and if it does happen, it screws up calculations. Defining this division float and then multiplying
		it in is not only faster on the CPU, but it gets rid of that zero problem.
	*/
	float divideByDirectionX = 1 / D.x;
	float divideByDirectionY = 1 / D.y;
	float divideByDirectionZ = 1 / D.z;
	
	if (divideByDirectionX >= 0) {
		tmin = (-0.5 - p.x) * divideByDirectionX;
		tmax = ( 0.5 - p.x) * divideByDirectionX;
	} else {
		tmin = ( 0.5 - p.x) * divideByDirectionX;
		tmax = (-0.5 - p.x) * divideByDirectionX;
	}
	
	if (divideByDirectionY >= 0) {
		tymin = (-0.5 - p.y) * divideByDirectionY;
		tymax = ( 0.5 - p.y) * divideByDirectionY;
	} else {
		tymin = ( 0.5 - p.y) * divideByDirectionY;
		tymax = (-0.5 - p.y) * divideByDirectionY;
	}
	
	if ( (tmin > tymax) || (tymin > tmax) ) return -1.0;

	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;
	
	if (divideByDirectionZ >= 0) {
		tzmin = (-0.5 - p.z) * divideByDirectionZ;
		tzmax = ( 0.5 - p.z) * divideByDirectionZ;
	} else {
		tzmin = ( 0.5 - p.z) * divideByDirectionZ;
		tzmax = (-0.5 - p.z) * divideByDirectionZ;
	}

	if ( (tmin > tzmax) || (tzmin > tmax) ) return -1.0;
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;

	//-10000000 -> 10000000 is maybe not what i want... it's supposed to be t0 -> t1 where that's a valid intersection interval
	if ((tmin < 100000000) && (tmax > -100000000)) { 
		return tmin;
	}
	return -1;
}