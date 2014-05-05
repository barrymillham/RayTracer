/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#include "Box.h"

GeometryItem Box::geo;
vec3 Box::boxPoints[24];
vec3 Box::boxNormals[24];
unsigned int Box::boxIndices[36];
bool Box::staticInitialized = false;
unsigned int Box::uShaderColorPointer;

void Box::staticInitialize(AttribLocations attribs)
{
	// If already initialized, don't do so again
	if(staticInitialized) return;

	Box::uShaderColorPointer = attribs.u_color;

	geo.renderMode = GL_TRIANGLES;

	geo.points = boxPoints;
	geo.normals = boxNormals;
	geo.numVertices = 24;

	geo.indices = boxIndices;
	geo.numIndices = 36;

	//// Define points and colors arrays
	// Front face (blue)
	geo.points[0] = vec3(0.5f,0.5f,0.5f); geo.normals[0] = vec3(0,0,1);
	geo.points[1] = vec3(0.5f,-0.5f,0.5f); geo.normals[1] = vec3(0,0,1);
	geo.points[2] = vec3(-0.5f,-0.5f,0.5f); geo.normals[2] = vec3(0,0,1);
	geo.points[3] = vec3(-0.5f,0.5f,0.5f); geo.normals[3] = vec3(0,0,1);

	// Back face (blue)
	geo.points[4] = vec3(0.5f,0.5f,-0.5f); geo.normals[4] = vec3(0,0,-1);
	geo.points[5] = vec3(-0.5f,0.5f,-0.5f); geo.normals[5] = vec3(0,0,-1);
	geo.points[6] = vec3(-0.5f,-0.5f,-0.5f); geo.normals[6] = vec3(0,0,-1);
	geo.points[7] = vec3(0.5f,-0.5f,-0.5f); geo.normals[7] = vec3(0,0,-1);

	// Right face (red)
	geo.points[8] = vec3(0.5f,0.5f,-0.5f); geo.normals[8] = vec3(1,0,0);
	geo.points[9] = vec3(0.5f,-0.5f,-0.5f); geo.normals[9] = vec3(1,0,0);
	geo.points[10] = vec3(0.5f,-0.5f,0.5f); geo.normals[10] = vec3(1,0,0);
	geo.points[11] = vec3(0.5f,0.5f,0.5f); geo.normals[11] = vec3(1,0,0);

	// Left face (red)
	geo.points[12] = vec3(-0.5f,0.5f,0.5f); geo.normals[12] = vec3(-1,0,0);
	geo.points[13] = vec3(-0.5f,-0.5f,0.5f); geo.normals[13] = vec3(-1,0,0);
	geo.points[14] = vec3(-0.5f,-0.5f,-0.5f); geo.normals[14] = vec3(-1,0,0);
	geo.points[15] = vec3(-0.5f,0.5f,-0.5f); geo.normals[15] = vec3(-1,0,0);

	// Top face (green)
	geo.points[16] = vec3(0.5f,0.5f,-0.5f); geo.normals[16] = vec3(0,1,0);
	geo.points[17] = vec3(0.5f,0.5f,0.5f); geo.normals[17] = vec3(0,1,0);
	geo.points[18] = vec3(-0.5f,0.5f,0.5f); geo.normals[18] = vec3(0,1,0);
	geo.points[19] = vec3(-0.5f,0.5f,-0.5f); geo.normals[19] = vec3(0,1,0);

	// Bottom face (green)
	geo.points[20] = vec3(0.5f,-0.5f,-0.5f); geo.normals[20] = vec3(0,-1,0);
	geo.points[21] = vec3(0.5f,-0.5f,0.5f); geo.normals[21] = vec3(0,-1,0);
	geo.points[22] = vec3(-0.5f,-0.5f,0.5f); geo.normals[22] = vec3(0,-1,0);
	geo.points[23] = vec3(-0.5f,-0.5f,-0.5f); geo.normals[23] = vec3(0,-1,0);

	//// Define index buffer
	// Front face
	geo.indices[0] = 0; // first triangle
	geo.indices[1] = 1;
	geo.indices[2] = 2;
	geo.indices[3] = 2; // second triangle
	geo.indices[4] = 3;
	geo.indices[5] = 0;

	// Back face
	geo.indices[6] = 4; // first triangle
	geo.indices[7] = 5;
	geo.indices[8] = 6;
	geo.indices[9] = 6; // second triangle
	geo.indices[10] = 7;
	geo.indices[11] = 4;

	// Right face
	geo.indices[12] = 8; // first triangle
	geo.indices[13] = 9;
	geo.indices[14] = 10;
	geo.indices[15] = 10; // second triangle
	geo.indices[16] = 11;
	geo.indices[17] = 8;

	// Left face
	geo.indices[18] = 12; // first triangle
	geo.indices[19] = 13;
	geo.indices[20] = 14;
	geo.indices[21] = 14; // second triangle
	geo.indices[22] = 15;
	geo.indices[23] = 12;

	// Top face
	geo.indices[24] = 16; // first triangle
	geo.indices[25] = 17;
	geo.indices[26] = 18;
	geo.indices[27] = 18; // second triangle
	geo.indices[28] = 19;
	geo.indices[29] = 16;

	// Bottom face
	geo.indices[30] = 20; // first triangle
	geo.indices[31] = 21;
	geo.indices[32] = 22;
	geo.indices[33] = 22; // second triangle
	geo.indices[34] = 23;
	geo.indices[35] = 20;

	geo.initialize(attribs);

	staticInitialized = true;
}

void Box::initialize(vec3 boxColor)
{
	// If already initialized, don't do so again
	if(initialized)	return;

	Box::boxColor = boxColor;

	initialized = true;
}

float Box::testRayIntersection(vec3 p0, vec3 v0, mat4 tInv)
{
	//FIND 
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