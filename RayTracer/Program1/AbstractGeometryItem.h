/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#include "../glm/glm.hpp"

using glm::vec3;
using glm::mat4;

// Abstract base class for all geometry items.
// Only the "draw" operation is defined at this level, as a function that takes a transformation (world) matrix and should be
// called from paintGL().
// GeometryItem is derived from this, and implements an item defined by a set of vertices and a drawing mode.
// Table, Chair, and Floor are directly derived from this, and are implemented as compositions of Boxes (itself derived
// from GeometryItem).
class AbstractGeometryItem
{
public:
	virtual void draw(glm::mat4 transform) = 0;
	virtual float getUnitHeight() = 0;
	virtual float testRayIntersection(vec3 p0, vec3 v0, mat4 tInv) = 0;
};