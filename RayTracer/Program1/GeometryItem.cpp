/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#include "GeometryItem.h"

GeometryItem::GeometryItem() :
	initialized(false),
	points(0), normals(0), numVertices(0),
	indices(0), numIndices(0),
	// I have no idea what will happen if you actually attempt to draw with this.
	// I'm hoping it will crash, because that's actually the intended behavior
	// (derived classes should be setting this to something more sensible).
	renderMode(GL_ZERO)
{ }

GeometryItem::~GeometryItem()
{
	glDeleteBuffers(1, &vboPosition);
	glDeleteBuffers(1, &vboNormal);
	glDeleteBuffers(1, &ibo);
}

void GeometryItem::initialize(AttribLocations attribs)
{
	GeometryItem::attribs = attribs;

	// Delete old VBOs if we have something previously loaded
	if(initialized)
	{
		glDeleteBuffers(1, &vboPosition);
		glDeleteBuffers(1, &vboNormal);
		glDeleteBuffers(1, &ibo);
	}

	// Set up the points VBO:
	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(vec3), points, GL_STATIC_DRAW);

	// Ditto normals:
	glGenBuffers(1, &vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(vec3), normals, GL_STATIC_DRAW);

	// Now set up the index buffer:
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned int), indices, GL_STATIC_DRAW);

	initialized = true;
}

void GeometryItem::draw(mat4 transform)
{
	if(!initialized)
	{
		SceneGraphException ex;
		ex.reason = "GeometryItem: must call initialize() before drawing!";
		throw ex;
	}

	// Bind the shader's attribute pointers to the VBOs used by this geometry item
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glEnableVertexAttribArray(attribs.v_pos);
	glVertexAttribPointer(attribs.v_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glEnableVertexAttribArray(attribs.v_normal);
	glVertexAttribPointer(attribs.v_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Send the model transformation matrix to the GPU as a uniform (uShaderModelMatrixPointer)
	glUniformMatrix4fv(attribs.u_model, 1, GL_FALSE, &transform[0][0]);

	// Draw the item!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(renderMode, numIndices, GL_UNSIGNED_INT, 0);
}