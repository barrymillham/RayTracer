/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#include "GeometryItem.h"

// Unit cube
class Box : public AbstractGeometryItem
{
public:
	// Implementations of abstract methods from AbstractGeometryItem:
	virtual void draw(mat4 transform)
	{
		if(!initialized)
		{
			SceneGraphException ex;
			ex.reason = "Box: must call initialize() before drawing!";
			throw ex;
		}

		glUniform3fv(uShaderColorPointer, 1, &Box::boxColor[0]);

		geo.draw(transform);
	}

	virtual float getUnitHeight() { return 1.0f; }

	// Default constructor - nothing to see here, move along people
	Box() : initialized(false)
	{ }

	// Instantiates point, color, and index arrays, and initializes the base class.
	void initialize(vec3 boxColor);

	// Sets up the VBOs for vertices and normals.
	// We only need one of these, which can be shared by all Box instances, so this
	// is a static member. This should be called in initializeGL().
	static void staticInitialize(AttribLocations attribs);

private:
	// Tracks whether this instance is ready to be drawn (i.e. non-static members
	// have been initialized).
	bool initialized;

	vec3 boxColor;
	static unsigned int uShaderColorPointer;

	static GeometryItem geo;
	static vec3 boxPoints[24];
	static vec3 boxNormals[24];
	static unsigned int boxIndices[36];

	static bool staticInitialized;
};