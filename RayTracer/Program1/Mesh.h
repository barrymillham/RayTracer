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
#include <cstdlib>
#include <ctime>

#include "Drawing.h"
#include "ExceptionClasses.h"

class Mesh
{
public:
	// Forward declarations since Face, Vertex, and HalfEdge all cross-reference each other
	struct Face;
	struct Vertex;
	struct HalfEdge;

	struct Face
	{
		vec3 p0, p1, p2; // TODO: remove these, these are only useful for the old face list structure and are in place so we don't break things yet
		Face() // also to be removed once half-edge is fully implemented
		{ }


		vec3 normal;

		HalfEdge *halfEdge;
		unsigned uid;

		// Remove the face from the mesh.
		// (Note: this does not actually remove the face from the mesh's internal storage. It will simply be skipped over when creating
		// an index buffer, and thus not drawn.)
		void deleteFace();

		Face(HalfEdge *halfEdge, vec3 normal) : halfEdge(halfEdge), normal(normal)
		{
			srand(time(0));
			uid = (unsigned)rand();
		}
	};

	struct Vertex
	{
		vec3 pos;
		vec3 color; // not used at the moment because we don't have a color VBO (it was removed when per-object colors were implemented...might want to rethink this)

		HalfEdge *halfEdge;
		unsigned uid;

		// The index of this vertex in the vertex buffer (used to build an index buffer)
		// (Should only be set and used internally by Mesh and Vertex methods.)
		unsigned internalIndex;

		Vertex(HalfEdge *halfEdge, vec3 pos) : halfEdge(halfEdge), pos(pos)
		{
			srand(time(0));
			uid = (unsigned)rand();
		}

		// Computes the normal for this vertex as the average of the face normals it's associated with
		vec3 getNormal();
	};

	struct HalfEdge
	{
		Vertex *vertex;
		HalfEdge *next; // next in CCW order on this face
		HalfEdge *sym;
		Face *face;

		HalfEdge(Vertex *vertex, HalfEdge *next, HalfEdge *sym, Face *face) : vertex(vertex), next(next), sym(sym), face(face)
		{ }
	};

	Mesh() : buffered(false)
	{ }

	~Mesh()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &nbo);
		glDeleteBuffers(1, &ibo);
	}

	void addFace(HalfEdge *halfEdge, vec3 normal)
	{
		faces.push_back(Face(halfEdge, normal));
	}

	// DEPRECATED - from face-list implementation
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

	// DEPRECATED - from face-list implementation
	// (the half-edge version will be similar, but should probably return a pointer instead)
	Face getFace(int index)
	{
		return faces[index];
	}

	// TODO: update this for half-edge
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

	// Build an index buffer for this mesh (using vertex normals)
	// Note: the std::vector indexBuffer will be emptied first, if there's anything in it.
	void fillIndexBuffer(std::vector<unsigned> &indexBuffer);

	// TODO: update for half-edge
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

	void subDivide() {
		//Mesh newMesh = Mesh();

		//int i, j, a, b, c, d;
		//Face f;
		//Vertex v, x;
 
		
		
		/*foreach (i, f, m->f) {
			foreach(j, v, f->v) {
				_get_idx(a, updated_point(v));
				_get_idx(b, edge_point(elem(f->e, (j + 1) % len(f->e))));
				_get_idx(c, face_point(f));
				_get_idx(d, edge_point(elem(f->e, j)));
				model_add_face(nm, 4, a, b, c, d);
			}
		}*/

	}

private:
	std::vector<Face> faces;
	std::vector<Vertex> vertices;
	std::vector<HalfEdge> halfEdges;

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<unsigned> indices;

	bool buffered;
	AttribLocations attribs;

	unsigned vbo, nbo, ibo;
};