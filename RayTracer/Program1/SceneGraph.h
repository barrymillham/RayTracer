/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#include <vector>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#include "AbstractGeometryItem.h"
#include "ExceptionClasses.h" // not using this yet in SceneGraph
#include "Drawing.h"


using glm::mat4;
using glm::vec3;

class SceneGraph
{
public:
	// A node in the scene graph tree
	struct Node
	{
	public:
		// Constructor
		// geo: pointer to an AbstractGeometryitem describing an item to be included
		//		in the scene; may be null (in which case this is a transofmration-
		//		only node)
		//		If not null, the pointer is expected to remain valid for the life of
		//		the Node (i.e. memory management is the caller's responsibility).
		// transform: transformation matrix for this node

		//Node(AbstractGeometryItem *geo, mat4 transform) : geo(geo), transform(transform)
		Node(AbstractGeometryItem *geo, vec3 r, vec3 t, vec3 s) : geo(geo), rotations(r), translations(t), scalings(s)
		{ 
			selected = false;
			yTrans = 0.0;	
		}

		~Node() { // Destructor deallocates all child nodes.
			for(int i = 0; i < children.size(); i++)
				delete children[i];
		}

		// Adds a child of this node.
		// *** NOTE: any nodes added with this function become the responsibility of
		// this Node. It will deallocate all children upon destruction.
		void addChild(Node *child) {
			children.push_back(child);
		}

		//Used to translate the Scenegraph Node up by the appropriate amount
		void setYTrans(float h) {
			yTrans = h;
		}
		float getYTrans() {
			return yTrans;
		}
		
		float testRayIntersection(vec3 p0, vec3 v0, mat4 parentTransformation) {
			mat4 nodeTransform = getTransformationMatrix();
			mat4 transformation = parentTransformation * nodeTransform;

			//If node has geometry, test it's geom for an intersection. else, intersection = -1
			float intersection = (geo)? geo->testRayIntersection(p0,v0,transformation) : -1;
			
			//intersection = +x or -1
			if (children[0] == NULL) return intersection;
	
			else { 
				//iterate through children, record minimum intersection > 0 and return it
				for (int i = 0; i < children.size(); i++) {
					float childIntersection = children[i]->getGeometry()->testRayIntersection(p0, v0, transformation);
					if (childIntersection != -1) //If there was an intersection 
						if (intersection == -1 || childIntersection < intersection)  //If it's closer than the previous intersection
							intersection = childIntersection;	//Make it the new "closest intersection"
				}
				return intersection;
			}
			return -1.0; // failsafe
		}
		
		// Draw the node, and all of its child nodes (preorder traversal).
		// parentTransform: transformation matrix of the parent node, which will
		//		be composed with this node's transformation for drawing and passed
		//		on to its children in similar fashion
		void draw(mat4 parentTransform)
		{
			mat4 transform = getTransformationMatrix();
			mat4 composition = parentTransform * transform;
			if(geo) {			
				if(selected) {
					glUniform1i(attribs.u_ambientOnly, 1);
					geo->draw(composition);
					glUniform1i(attribs.u_ambientOnly, 0); // re-enable advanced lighting for the rest of the objects
				}
				else geo->draw(composition);
			}
			for(int i = 0; i < children.size(); i++)
				children[i]->draw(composition);
		}

		mat4 getTransformationMatrix() {
			//construct temporary matrix using stored values
			mat4 scaleMat = glm::scale(mat4(1.0f), scalings);
			mat4 rotXMat = glm::rotate(mat4(1.0f), rotations.x, vec3(1,0,0));
			mat4 rotYMat = glm::rotate(mat4(1.0f), rotations.y, vec3(0,1,0));
			mat4 rotZMat = glm::rotate(mat4(1.0f), rotations.z, vec3(0,0,1));
			mat4 transMat = glm::translate(mat4(1.0f), translations);
			return (transMat * rotZMat * rotYMat * rotXMat * scaleMat);
		}
		void setSelected(bool s) {selected = s;}
		bool getSelected() {return selected;}
		int getRotationDegreesY() {return rotations.y;}
		void setRotationDegreesY(int r) {rotations.y = r;}
		float getScalingX() {return scalings.x;}
		void setScalingX(float s) {scalings.x = s;}
		float getScalingY() {return scalings.y;}
		void setScalingY(float s) {scalings.y = s;}
		float getScalingZ() {return scalings.z;}
		void setScalingZ(float s) {scalings.z = s;}
		float getTranslationX() {return translations.x;}
		void setTranslationX(float t) {translations.x = t;}
		float getTranslationY() {return translations.y;}
		void setTranslationY(float t) {translations.y = t;}
		float getTranslationZ() {return translations.z;}
		void setTranslationZ(float t) {translations.z = t;}

		AbstractGeometryItem* getGeometry() { return geo; }


	private:
		AbstractGeometryItem *geo; // null if this is a transformation-only node
		//mat4 transform;
		std::vector<Node*> children; // empty if this is a leaf
		bool selected;
		float yTrans;

		//values modified by UI sliders
		//int rotationDegrees;
		vec3 rotations;
		vec3 translations;
		vec3 scalings;
	};

	// Constructor
	// The head of the scene graph has null geometry and an identity transform.
	// The rest of the tree is attached to this node. (see addChildToHead())
	SceneGraph()
	{
		//head = new Node(0, mat4(1.0f)); // null geometry, identity matrix
		head = new Node(0, vec3(0,0,0), vec3(0,0,0), vec3(1,1,1)); // null geometry, identity transformations
	}

	// Destructor
	// Deallocates the head node, which in turn causes the deallocation of all
	// its child nodes.
	~SceneGraph()
	{
		delete head;
	}

	// Removes all nodes from the scene graph, leaving just the head node.
	void clear()
	{
		delete head;
		head = new Node(0, vec3(0,0,0), vec3(0,0,0), vec3(1,1,1)); // null geometry, identity transformations
	}

	// Adds a child of the head of the scene graph.
	// *** NOTE: any nodes added with this function become the responsibility of
	// this Node. It will deallocate all children upon destruction.
	void addChildToHead(Node *child)
	{
		head->addChild(child);
	}

	Node* getHead(){return head;}

	// Draws the scene (traversing from the head node)
	void draw(mat4 m = mat4(1.0f))
	{
		/*mat4 translation = glm::translate(m, glm::vec3(0.0f,head->getYTrans(),0.0f));
		m = (m) * (translation);*/
		head->draw(m);
	}

	

private:
	Node *head;

	// Copy constructor - SceneGraphs should not be copied
	SceneGraph(const SceneGraph &s)
	{
		SceneGraphException ex;
		ex.reason = "SceneGraph: scene graph objects cannot be copied!";
		throw ex;
	}
};