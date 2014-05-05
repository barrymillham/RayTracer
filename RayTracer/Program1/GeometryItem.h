/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#define GLEW_STATIC
#include "glew.h"
#include "../glm/glm.hpp"

#include "AbstractGeometryItem.h"
#include "Drawing.h"
#include "ExceptionClasses.h"

using glm::vec3;
using glm::mat4;

// Generic geometry item.
// Specific geometry should be defined in derived classes.
class GeometryItem
{
public:
	// Draws the geometry item according to the specified model transformation matrix.
	// Expects a compatible shader program to be loaded and ready for use.
	// *** initialize() must have been called prior to this! If this has not been done,
	// draw() will throw a SceneGraphException.
	virtual void draw(mat4 transform);

	// Default constructor
	// This should only ever be called from derived class
	// constructors (plain GeometryItems should not be instantiated).
	// Note that this constructor only initializes members to zero (or similar)
	// for safety; derived types still need to set them to something sensible
	// for drawing.
	GeometryItem();

	// Destructor - deletes VBOs
	~GeometryItem();

	// Initializes global objects for drawing instances of this geometry item:
	// * Creates vertex buffer objects for position and color
	// * Loads the vertex buffers into VRAM
	// Only needs to be called once, at the beginning of the program (probably in
	// initializeGL()) - MUST be called before draw()!
	// (This should only be called from a corresponding initialize() function in
	// derived classes. You will probably want to make the derived initialize() public,
	// since this isn't.)
	//		-Note that the derived initialize() function will need to initialize points
	//		and colors *before* calling this, since they're sent to VRAM here.
	void initialize(AttribLocations attribs);

	// Vertex data arrays for the geometry item. Derived classes are expected
	// to point these at arrays of the appropriate size. (Allocation and destruction of
	// these arrays are the responsibility of the derived class.)
	vec3 *points;
	// Normals associated with each vertex
	vec3 *normals;
	int numVertices; // number of vertices (and colors, normals...)
	
	unsigned int *indices;
	int numIndices;

	// This should be set in the constructor of a derived class.
	GLenum renderMode;

private:
	bool initialized; // initialized to false in GeometryItem.cpp
	AttribLocations attribs;

	unsigned int vboPosition, vboNormal;
	unsigned int ibo;
};