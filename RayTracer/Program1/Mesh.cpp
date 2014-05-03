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

void Mesh::bufferData(AttribLocations attribs)
{
	Mesh::attribs = attribs; // we don't actually need these in this function, but it's a good place to get it from upstream for later use in draw()

	// Delete old VBOs if we have something previously loaded
	if(buffered)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &nbo);
		glDeleteBuffers(1, &ibo);
	}

	// Create "raw data" buffers based on the data stored in our half-edge structure
	std::vector<vec3> positions;
	for(int i = 0; i < vertices.size(); i++)
		positions.push_back(vertices[i].pos);

	std::vector<vec3> normals;
	for(int i = 0; i < vertices.size(); i++)
		normals.push_back(vertices[i].getNormal());

	fillIndexBuffer(indices); // note: fillIndexBuffer() automatically clears anything previously left in indices, which is what we want

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

void Mesh::draw(mat4 transform)
{
	if(!buffered)
	{
		MeshException e;
		e.reason = "Mesh: must call bufferData() before drawing!";
		throw e;
	}

	// Bind the shader's attribute pointers to the VBOs used by this mesh
	// (we need to do this here for multiple meshes, and other geometry objects, to coexist)
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