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

using glm::vec3;
using glm::vec4;
using glm::mat4;

#include <vector>

#include "Drawing.h"
#include "ExceptionClasses.h"

class Mesh
{
public:
	struct Face // triangular face structure
	{
		vec3 p0, p1, p2; // the three points of the triangle
		vec3 normal; // the normal for this face
		vec4 color; // the color the face should be drawn (to be used later - right now we're specifying color per-object, i.e. for the whole mesh, as a uniform)
	};

	Mesh() : buffered(false)
	{ }

	~Mesh()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &nbo);
		glDeleteBuffers(1, &ibo);
	}

	void addFace(Face face)
	{
		faces.push_back(face);

		positions.push_back(face.p0);
		positions.push_back(face.p1);
		positions.push_back(face.p2);
		for(int i = 0; i < 3; i++)
		{
			normals.push_back(face.normal);
			indices.push_back(indices.size());
		}
	}

	Face getFace(int index)
	{
		return faces[index];
	}

	// Remove all faces from the mesh structure
	void clear()
	{
		faces.clear();
		positions.clear();
		normals.clear();

		if(buffered)
		{
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &nbo);
			glDeleteBuffers(1, &ibo);
		}

		buffered = false;
	}

	void bufferData(AttribLocations attribs)
	{
		Mesh::attribs = attribs;

		// Delete old VBOs if we have something previously loaded
		if(buffered)
		{
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &nbo);
			glDeleteBuffers(1, &ibo);
		}

		// Generate and fill new buffers
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(vec3), positions.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &nbo);
		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(vec3), normals.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

		buffered = true;
	}

	void draw(mat4 transform)
	{
		if(!buffered)
		{
			MeshException e;
			e.reason = "Mesh: must call bufferData() before drawing!";
			throw e;
		}

		// Bind the shader's attribute pointers to the VBOs used by this mesh
		// (this is needed for multiple meshes, and other geometry objects, to coexist)
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(attribs.v_pos);
		glVertexAttribPointer(attribs.v_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		glEnableVertexAttribArray(attribs.v_normal);
		glVertexAttribPointer(attribs.v_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Send the model matrix, and a color for the mesh, to the GPU as uniforms
		glUniformMatrix4fv(attribs.u_model, 1, GL_FALSE, &(mat4(1.0f))[0][0]);
		glUniform3f(attribs.u_color, 1.0f, 0.0f, 0.0f); // set color to red (TODO: don't hardcode this)
		//glUniform1i(attribs.u_ambientOnly, 1); // turn off advanced lighting (for debugging - comment out under normal circumstances)

		// Draw the mesh
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

private:
	std::vector<Face> faces;

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<unsigned> indices;

	bool buffered;
	AttribLocations attribs;

	unsigned vbo, nbo, ibo;
};