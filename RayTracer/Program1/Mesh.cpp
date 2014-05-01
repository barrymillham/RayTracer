#include "Mesh.h"

void Mesh::Face::deleteFace()
{
	// For original pseudocode, see notes: "[2014-04-07] Subdivision.pptx", slide 20

	// Traverse the halfedges associated with this face:
	// * If a vertex points to this HE, point it to the symmetric HE on another face instead.
	// * Point the symmetric HE's sym pointer to null.
	HalfEdge *he = halfEdge;
	do
	{
		if(he->vertex != 0)
			he->vertex->halfEdge = he->next->sym;
		he->sym->sym = 0;

		he = he->next;
	} while(he != halfEdge); // loop until we've gone around to the halfedge we started at
}

vec3 Mesh::Vertex::getNormal()
{
	// For original pseudocode, see notes: "[2014-04-07] Subdivision.pptx", slide 17

	// Start with the normal from the face we have a pointer to
	vec3 sumNormals = halfEdge->face->normal;

	// Step through all the other faces this vertex is part of, adding them to ours
	HalfEdge *he = halfEdge->next->sym;
	while(he != halfEdge)
	{
		sumNormals += he->face->normal;
		he = he->next->sym;
	}

	// Renormalize our normal to get the average of the face normals around this point
	return glm::normalize(sumNormals);
}

void Mesh::fillIndexBuffer(std::vector<unsigned> &indexBuffer)
{
	// For original pseudocode, see notes: "[2014-04-07] Subdivision.pptx", slide 18

	indexBuffer.clear();

	// For each face, get a halfedge associated with it, and traverse he.next to determine all the vertices for the face
	for(int i = 0; i < faces.size(); i++)
	{
		HalfEdge *he = faces[i].halfEdge;
		do
		{
			indexBuffer.push_back(he->vertex->internalIndex);
			he = he->next;
		} while(he != faces[i].halfEdge); // loop until we've gone around to the halfedge we started at
	}
}