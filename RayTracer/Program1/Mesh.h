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

#include "AbstractGeometryItem.h"
#include "Drawing.h"
#include "ExceptionClasses.h"

class Mesh : public AbstractGeometryItem
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
		Vertex *centerPoint;

		std::vector<Vertex*> newVertices;

		// Remove the face from the mesh.
		// (Note: this does not actually remove the face from the mesh's internal storage. It will simply be skipped over when creating
		// an index buffer, and thus not drawn.)
		void deleteFace();

		Face(HalfEdge *halfEdge, vec3 normal) : halfEdge(halfEdge), normal(normal), centerPoint(&Vertex())
		{
			srand(time(0));
			uid = (unsigned)rand();
		}

		// Get the index of this face in the mesh's face array, which can be used as a handle to it for later operations
		int getIndex()
		{
			return internalIndex;
		}

	private:
		friend class Mesh;

		// The index at which this face is stored in the mesh's face array
		// (The client might need this to access the face later, and should use getIndex() for this.)
		int internalIndex;

		// To enable defining a total ordering of faces (mainly useful for putting them in sets, maps, etc. since the ordering is meaningless)
		unsigned uid;
	};

	struct Vertex
	{
		vec3 pos;
		vec3 color; // not used at the moment because we don't have a color VBO (it was removed when per-object colors were implemented...might want to rethink this)

		HalfEdge *halfEdge;

		Vertex():halfEdge(NULL), pos(vec3(-1,-1,-1)){}

		Vertex(HalfEdge *halfEdge, vec3 pos) : halfEdge(halfEdge), pos(pos)
		{
			srand(time(0));
			uid = (unsigned)rand();
		}

		// Get the index of this vertex in the mesh's vertex buffer array, which can be used as a handle to it for later operations
		int getIndex()
		{
			return internalIndex;
		}

		// Computes the normal for this vertex as the average of the face normals it's associated with
		vec3 getNormal();

	private:
		friend class Mesh;

		// The index at which this vertex is stored in the mesh's vertex buffer array
		// (The client might need this to access the face later, and should use getIndex() for this; also used by Mesh to build an index buffer.)
		int internalIndex;

		// To enable defining a total ordering of vertices (mainly useful for putting them in sets, maps, etc. since the ordering is meaningless)
		unsigned uid;
	};

	struct HalfEdge
	{
		Vertex *vertex;
		HalfEdge *next; // next in CCW order on this face
		HalfEdge *sym;
		Face *face; 
		Vertex *halfVertex;

		HalfEdge(Vertex *vertex, HalfEdge *next, HalfEdge *sym, Face *face) : vertex(vertex), next(next), sym(sym), 
			face(face), halfVertex(NULL)
		{
			srand(time(0));
			uid = (unsigned)rand();
		}

		// Get the index of this halfedge in the mesh's halfedge array, which can be used as a handle to it for later operations
		int getIndex()
		{
			return internalIndex;
		}

	private:
		friend class Mesh;

		// The index at which this face is stored in the mesh's face array
		// (The client might need this to access the face later, and should use getIndex() for this.)
		int internalIndex;

		// To enable defining a total ordering of faces (mainly useful for putting them in sets, maps, etc. since the ordering is meaningless)
		unsigned uid;
	};

	Mesh() : buffered(false)
	{ }

	~Mesh()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &nbo);
		glDeleteBuffers(1, &ibo);
	}

	int addFace(HalfEdge *halfEdge, vec3 normal)
	{
		Face newFace(halfEdge, normal);
		newFace.internalIndex = faces.size();
		faces.push_back(newFace);

		return newFace.internalIndex;
	}

	// Returns the internal index of the newly-created vertex
	int addVertex(HalfEdge *halfEdge, vec3 pos)
	{
		Vertex newVertex(halfEdge, pos);
		newVertex.internalIndex = vertices.size();
		vertices.push_back(newVertex);

		return newVertex.internalIndex;
	}

	int addHalfEdge(Vertex *vertex, HalfEdge *next, HalfEdge *sym, Face *face)
	{
		HalfEdge newHalfEdge(vertex, next, sym, face);
		newHalfEdge.internalIndex = halfEdges.size();
		halfEdges.push_back(newHalfEdge);

		return newHalfEdge.internalIndex;
	}

	// Note: we *don't* have corresponding delete functions because this would require shifting back all successive elements of the respective
	// array, which would invalidate all handles to successive items (not to mention that it'd be a very slow operation for large meshes).

	Face* getFace(int index)
	{
		return &faces[index];
	}

	Vertex* getVertex(int index)
	{
		return &vertices[index];
	}

	HalfEdge* getHalfEdge(int index)
	{
		return &halfEdges[index];
	}

	// Determine the center of a face
	Vertex getCenterPoint(Face* face);

	Vertex* getPreviousVertex(HalfEdge* he) {
		HalfEdge current = *he;
		while(current.next != he)  // loop until we've gone around to the halfedge before the halfedge we started at
			current = *current.next;
		return (he->vertex);
	}

	// Remove all faces from the mesh structure
	void clear()
	{
		faces.clear();
		vertices.clear();
		halfEdges.clear();

		indices.clear();

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

	// Generate VBOs on the GPU based on the data stored in our half-edge structure
	void bufferData(AttribLocations attribs);

	void subDivide();

	// Abstract functions inherited from AbstractGeometryItem
	virtual void draw(mat4 transform);
	virtual float getUnitHeight(); // Calculate the height of the mesh (maximum - minimum points in y-dimension)

private:
	std::vector<Face> faces;
	std::vector<Vertex> vertices;
	std::vector<HalfEdge> halfEdges;

	std::vector<unsigned> indices;

	bool buffered;
	AttribLocations attribs;
	float height;
	unsigned vbo, nbo, ibo;
};