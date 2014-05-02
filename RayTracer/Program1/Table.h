/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#include "../glm/gtc/matrix_transform.hpp"

#include "AbstractGeometryItem.h"
#include "Box.h"

using glm::mat4;
using glm::vec3;
using glm::scale;
using glm::translate;

class Table : public AbstractGeometryItem
{
public:
	void initialize(vec3 tableColor)
	{
		box.initialize(tableColor);
	}

	// Pure virtual function inherited from AbstractGeometryItem
	virtual void draw(mat4 transform)
	{
		mat4 top_scale = scale(mat4(1.0f), vec3(1.0f, 0.2f, 1.0f));
		mat4 top_tr = translate(mat4(1.0f), vec3(0.0f, 0.5f, 0.0f));
		box.draw(transform * top_tr * top_scale);

		mat4 legTrans = scale(mat4(1.0f), vec3(0.05f, 1.0f, 0.05f));
		mat4 frontLeftLegTrans = translate(mat4(1.0f), vec3(-0.475f, 0.0f, 0.475f));
		mat4 frontRightLegTrans = translate(mat4(1.0f), vec3(0.475f, 0.0f, 0.475f));
		mat4 backLeftLegTrans = translate(mat4(1.0f), vec3(-0.475f, 0.0f, -0.475));
		mat4 backRightLegTrans = translate(mat4(1.0f), vec3(0.475f, 0.0f, -0.475f));

		box.draw(transform * frontLeftLegTrans * legTrans);
		box.draw(transform * frontRightLegTrans * legTrans);
		box.draw(transform * backLeftLegTrans * legTrans);
		box.draw(transform * backRightLegTrans * legTrans);
	}

private:
	Box box;
};