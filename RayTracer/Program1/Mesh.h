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

	void draw(mat4 transform);

	Vertex getCenterPoint(Face* face) {
		
		std::vector<vec3> positions;
			
		HalfEdge *he = face->halfEdge;
		do
		{
			positions.push_back(he->vertex->pos);
		} while(he != face->halfEdge); // loop until we've gone around to the halfedge we started at
		Vertex newVertex(he, vec3(0,0,0));
		for (int j = 0; j < positions.size(); j++) {
			newVertex.pos.x += positions[j].x;
			newVertex.pos.y += positions[j].y;
			newVertex.pos.z += positions[j].z;
		}
		newVertex.pos /= positions.size();
		return newVertex;
	}

	Vertex* getPreviousVertex(HalfEdge* he) {
		HalfEdge current = *he;
		while(current.next != he)  // loop until we've gone around to the halfedge before the halfedge we started at
			current = *current.next;
		return (he->vertex);
	}

	void subDivide() {
		Mesh newMesh = *this; // Maybe have to write a copy constructor for this to work as we would like
		
		//All vertices that were in the original mesh are stored in the oldVertices vector
		//to give us a reference for what vertices will be moved later on
		std::vector<Vertex*> oldVertices;
		for (int i = 0; i < newMesh.halfEdges.size(); i++) 
			oldVertices.push_back(newMesh.halfEdges[i].vertex);
		
		//This will be filled as new vertices are created so that we can ensure that these vertices are pointing 
		//to the correct half-edge at the end.
		//std::vector<Vertex*> newVertices;

		/* Next we find the center points of each face by averaging the positions of all the vertices 
			that make up the face. Store that center vertex as a temporary member of the face. */
		for (int i = 0; i < newMesh.faces.size(); i++) {
			Face* currentFace = &newMesh.faces[i];
			Vertex centerVertex = getCenterPoint(currentFace);
			currentFace->centerPoint = &centerVertex;
			/********** I foresee a problem with centerPoint being added to the vertex but then going out of scope
			so now the vector just sees a junk vertex.
			*/
		}

		/* Next create all new vertices that will lay on the current edges.
			These vertices are given a position (obviously), and a halfEdge. The halfedge that the vertex
			points to is the halfedge that it lies on. The halfEdge that the vertex lies on also is given
			a temporary pointer to that vertex as well.
		*/
		/***********	 NOTE: Remember to change the new vertices to contain a pointer to the halfedge 
		 = halfedge->next->sym (where halfedge is the original halfedge that pointed to the vertex)
		**/
		for (int i = 0; i < newMesh.halfEdges.size(); i++) {
			Vertex edgeVertex;
			if (newMesh.halfEdges[i].halfVertex != NULL || newMesh.halfEdges[i].sym->halfVertex != NULL) continue; //if the half edge already has a vertex there.
			
			/************If Sym can never be null, then there will always be 4 points at the center of a face (Erase the else)**/
			if (halfEdges[i].sym != NULL) { 
				//average vertices on all four sides of half edge
				Vertex v1 = *newMesh.halfEdges[i].vertex;
				Vertex v2 = *getPreviousVertex(&newMesh.halfEdges[i]);
				Vertex v3 = *newMesh.halfEdges[i].face->centerPoint;
				Vertex v4 = *newMesh.halfEdges[i].sym->face->centerPoint;
				edgeVertex.pos = (v1.pos + v2.pos + v3.pos + v4.pos) / 4.0f;

			//MIGHT NOT NEED THIS ELSE IF THERE WILL NEVER BE A NULL "SYM"
			}else { //outer half edge
				//Only use vertices on either side of the halfedge
				Vertex v1 = *newMesh.halfEdges[i].vertex;
				Vertex v2 = *getPreviousVertex(&newMesh.halfEdges[i]);	
				edgeVertex.pos = (v1.pos + v2.pos)/2.0f;
			}
			//A vertex needs to store a position and a halfEdge
			edgeVertex.halfEdge = &newMesh.halfEdges[i]; //add a halfedge to the vertex
			newMesh.halfEdges[i].halfVertex = &edgeVertex; //add the vertex to the halfedge (temporarily)
			newMesh.halfEdges[i].sym->halfVertex = &edgeVertex; //add the vertex to the halfedge's sym (temporarily)

			//store the two faces for this in the face's newVertices vector
			edgeVertex.halfEdge->face->newVertices.push_back(&edgeVertex);
			edgeVertex.halfEdge->sym->face->newVertices.push_back(&edgeVertex);
			
			//how do i set it for a face, when we're looping through half edges?
			/****** I foresee a problem with adding edgeVertex to the vector and then it going out of scope so
			the vector is left pointing at a junk vertex.
			*/
		}

		/* Next create the halfedges that will border the existing faces (Halfedges that go from the original 
			vertices to the edge half-vertices). In this step, no halfedges will be created that go from the center 
			of the face out to the edges. That comes later (after creating the faces).
		*/
		for (int i = 0; i < newMesh.faces.size(); i++) {
			HalfEdge* firstHe = newMesh.faces[i].halfEdge;

			//For each halfedge around the face.
			for (HalfEdge* originalHe = firstHe; originalHe->next != firstHe; originalHe = originalHe->next) {
				HalfEdge newHalfEdge = *originalHe; //Create halfedge identical to this halfedge
				newHalfEdge.halfVertex = NULL; //clear the new halfedge's halfvertex. Only the original needs it.
				//I foresee a problem with this HE going out of scope and so all the information is borked.

				//Update all the original half-edge data members to reference the new halfedge that is placed in 
				//	front of it.
				originalHe->vertex = originalHe->halfVertex;
				originalHe->halfVertex = NULL;
				originalHe->next = &newHalfEdge;
				originalHe->sym = originalHe->next->sym->next; 
				originalHe->face = originalHe->face; //Keep the same for now.
			}
		}


		/* Next create all the new faces that will be inside the original face. These faces will take the place
		of the one big face. At the end of this, make sure to set the originalFace->centerPoint equal to NULL.
		Don't forget to add a normal equal to the normal of the original face as well.
		Also set newVertices to null for all faces
		*/
		/***********	 NOTE: Remember to change the new vertices to contain a pointer to the halfedge 
		 = halfedge->next->sym (where halfedge is the original halfedge that pointed to the vertex)
		**/
		std::vector<Face*> newFaces;

		for (int i = 0; i < newMesh.faces.size(); i++) {
			
			//first create vector of new faces
			for(int j=0; j<newMesh.faces[i].newVertices.size(); j++)
			{
				Face* face;
				newFaces.push_back(face);
			}

			//first set the first HE you start at to point to face 1 (newFaces[0]) in the newFaces vec
			newMesh.faces[i].halfEdge->face = newFaces[0];
			//set newFaces[0]'s HE to this first HE
			newFaces[0]->halfEdge = newMesh.faces[i].halfEdge;
			HalfEdge* firstHe = newMesh.faces[i].halfEdge;
			HalfEdge* iterator = newMesh.faces[i].halfEdge;
			for(int j=1; j<newMesh.faces[i].newVertices.size(); j++)
			{
				//go to next HE, set face to newFaces[i]
				iterator = iterator->next;
				iterator->face = newFaces[i];
				//go to next HE, set face to newFaces[i]
				iterator = iterator->next;
				iterator->face = newFaces[i];
				//set newFaces[i]'s HE to this HE
				newFaces[i]->halfEdge = iterator;
			}
			//set last one to face 1 -- the next one should now be the first one
			iterator = iterator->next;
			iterator->face = newFaces[0];
		
		
			//NOW WE WILL CREATE THE NEW HE's for this face that are between the center point and the new vertices
			//note: newvertices should be by face. right now it is just a big list of all new vertices
			//temp vector of HE's that point out from middle to be used for connecting "nexts"
			std::vector<HalfEdge*> outwardHEs;
			std::vector<HalfEdge*> inwardHEs;
			for(int j = 0; j < newMesh.faces[i].newVertices.size(); j++) {
				//he1 and he2 are down and up
				HalfEdge he1(newMesh.faces[i].centerPoint, NULL, NULL, newFaces[j]); //face to the left -- NOT SURE IF THIS IS RIGHT FACE -- now face to the right.....now clap your hands
				HalfEdge he2(newMesh.faces[i].newVertices[j], NULL, &he1, newFaces[(j+1)%newMesh.faces[i].newVertices.size()]);//face to the right
				outwardHEs.push_back(&he2);
				inwardHEs.push_back(&he1);
				he1.sym = &he2;
				//loop through all outer HE, comparing their "vertex" to newVertices[j]
				//he2.next will equal THAT one's next
				for (HalfEdge* originalHe = firstHe; originalHe->next != firstHe; originalHe = originalHe->next) {
					if(newMesh.faces[i].halfEdge->vertex == newMesh.faces[i].newVertices[j])
						he2.next = newMesh.faces[i].halfEdge->next;
					//else major error
				}

				//for all the new faces, set the remaining unconnected "next" HE to be connected
				//This assumes that the first halfedge of a face matches up with the first item in the inwardHEs.
				for(int k=0; k<newFaces.size(); k++)
				{
					newFaces[i]->halfEdge->next = inwardHEs[i];
				}


				//DID WE UPDATE ALL HALFEDGES' FACES TO NEW FACES? AND EVERYTHING ELSE?????
					
				int temp = j;
				if((temp-1) < 0) temp = newMesh.faces[i].newVertices.size()-1;
				he1.next = outwardHEs[temp];
				newMesh.faces[i].centerPoint->halfEdge = &he1;
			}

		
		}



		



		/*
		By this point, the mesh should be in a state where it has been completely subdivided. All new vertices 
		have been added, all new halfedges have been added, all new faces have been added.
		*/

		/*
		Next, move all original vertices. Do this by looping through the vector of original vertices, 
			and move their positions to the average of the four surrounding vertices. The four surrounding vertices
			are the vertices that were created in the center of the edge during this CC iteration.
		*/
		for (int i = 0; i < oldVertices.size(); i++) {
			Vertex* vert = oldVertices[i];

			Vertex v1 = *getPreviousVertex(vert->halfEdge);					//Beneath
			Vertex v2 = *vert->halfEdge->next->vertex;						//Right
			Vertex v3 = *vert->halfEdge->next->sym->next->vertex;			//Up
			Vertex v4 = *vert->halfEdge->next->sym->next->sym->next->vertex;//Left
			
			vec3 newPos = (v1.pos+v2.pos+v3.pos+v4.pos)/4.0f;
			vert->pos = newPos;
		}




		//Finally, overwrite THIS mesh to be equal to the mesh that we just created!
		*this = newMesh; 
	}

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