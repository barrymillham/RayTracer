#include "Mesh.h"

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