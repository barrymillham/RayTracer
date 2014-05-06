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

int Mesh::splitFace(Face *f, Vertex *p1, Vertex *p2)
{
	// Find halfedges pointing to p1 and p2
	HalfEdge *he_p1 = p1->halfEdge;
	while(he_p1->vertex != p1)
		he_p1 = he_p1->next;
	HalfEdge *he_p2 = he_p1;
	while(he_p2->vertex != p2)
		he_p2 = he_p2->next;

	// Create two new halfedges on the split line
	int he1 = addHalfEdge(p2, he_p2->next, 0, 0);
	int he2 = addHalfEdge(p1, he_p1->next, 0, 0);
	HalfEdge *pHE1 = getHalfEdge(he1);
	HalfEdge *pHE2 = getHalfEdge(he2);

	// Create a new face
	int newFace = addFace(pHE1, f->normal);
	Face *pNewFace = getFace(newFace);

	// Point he1 to the new face, and he2 to the old one
	pHE1->face = pNewFace;
	pHE2->face = f;
	// Point the new face to he1, and the old face to he2
	pNewFace->halfEdge = pHE1;
	f->halfEdge = pHE2;

	// Point p1 to he1 and p2 to he2
	p1->halfEdge = pHE1;
	p2->halfEdge = pHE2;

	// Fix the next pointers for he_p1 and he_p2
	he_p1->next = pHE1;
	he_p2->next = pHE2;

	// he1 and he2 are symmetric to each other
	pHE1->sym = pHE2;
	pHE2->sym = pHE1;

	// We *should* be done now (assuming I remembered all the pointers! :-)) - return the index of the new face
	return newFace;
}

Mesh::Vertex Mesh::getCenterPoint(Face* face)
{
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

float Mesh::getUnitHeight()
{
	if(vertices.size() < 1)
		return 0.0f;

	float maxY = vertices[0].pos.y;
	float minY = vertices[0].pos.y;

	for(int i = 1; i < vertices.size(); i++)
	{
		if(vertices[i].pos.y > maxY)
			maxY = vertices[i].pos.y;
		if(vertices[i].pos.y < minY)
			minY = vertices[i].pos.y;
	}

	return maxY - minY;
}

void Mesh::subDivide()
{
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