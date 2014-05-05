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

class Chair : public AbstractGeometryItem
{
public:
	void initialize(vec3 chairColor)
	{
		box.initialize(chairColor);
	}

	// Pure virtual functions inherited from AbstractGeometryItem
	virtual void draw(mat4 transform)
	{
		mat4 seatTrans = translate(mat4(1.0f), vec3(0.0f, 0.5f, 0.0f));
		mat4 seatScale = scale(mat4(1.0f), vec3(1.0f, 0.2f, 1.0f));
		box.draw(transform * seatTrans * seatScale);

		mat4 backingScale = scale(mat4(1.0f), vec3(1.0f, 1.0f, 0.2f));
		mat4 backingTrans = translate(mat4(1.0f), vec3(0.0f, 1.1f, -0.3f));
		box.draw(transform * backingTrans * backingScale);

		mat4 legTrans = scale(mat4(1.0f), vec3(0.05f, 1.0f, 0.05f));
		mat4 frontLeftLegTrans = translate(mat4(1.0f), vec3(-0.475f, 0.0f, 0.475f));
		mat4 frontRightLegTrans = translate(mat4(1.0f), vec3(0.475f, 0.0f, 0.475f));
		mat4 backLeftLegTrans = translate(mat4(1.0f), vec3(-0.475f, 0.0f, -0.475f));
		mat4 backRightLegTrans = translate(mat4(1.0f), vec3(0.475f, 0.0f, -0.475f));
		
		box.draw(transform * frontLeftLegTrans * legTrans);
		box.draw(transform * frontRightLegTrans * legTrans);
		box.draw(transform * backLeftLegTrans * legTrans);
		box.draw(transform * backRightLegTrans * legTrans);
	}

	virtual float getUnitHeight()
	{
		return 2.2; //chair leg yScale = 1.0, seat yScale = 0.2, back yScale = 1.0, so chair height = 2.2
	}

private:
	Box box;
};