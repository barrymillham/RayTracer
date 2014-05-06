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

	// Pure virtual functions inherited from AbstractGeometryItem
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

	virtual float Table::testRayIntersection(vec3 p0, vec3 v0, mat4 tInv)
	{
		tInv = glm::inverse(tInv);
		std::vector<float> tValues;

		mat4 top_scale = scale(mat4(1.0f), vec3(1.0f, 0.2f, 1.0f));
		mat4 top_tr = translate(mat4(1.0f), vec3(0.0f, 0.5f, 0.0f));
		tValues.push_back(box.testRayIntersection(p0, v0, tInv * glm::inverse(glm::inverse(tInv) * top_tr * top_scale)));

		mat4 legTrans = scale(mat4(1.0f), vec3(0.05f, 1.0f, 0.05f));
		mat4 frontLeftLegTrans = translate(mat4(1.0f), vec3(-0.475f, 0.0f, 0.475f));
		mat4 frontRightLegTrans = translate(mat4(1.0f), vec3(0.475f, 0.0f, 0.475f));
		mat4 backLeftLegTrans = translate(mat4(1.0f), vec3(-0.475f, 0.0f, -0.475));
		mat4 backRightLegTrans = translate(mat4(1.0f), vec3(0.475f, 0.0f, -0.475f));

		tValues.push_back(box.testRayIntersection(p0, v0, tInv * glm::inverse(glm::inverse(tInv) * frontLeftLegTrans * legTrans)));
		tValues.push_back(box.testRayIntersection(p0, v0, tInv * glm::inverse(glm::inverse(tInv) * frontRightLegTrans * legTrans)));
		tValues.push_back(box.testRayIntersection(p0, v0, tInv * glm::inverse(glm::inverse(tInv) * backLeftLegTrans * legTrans)));
		tValues.push_back(box.testRayIntersection(p0, v0, tInv * glm::inverse(glm::inverse(tInv) * backRightLegTrans * legTrans)));

		//return smallest intersection point (or -1 if no intersection)
		if(tValues.size() == 0) return -1;
		else {
			float temp = tValues[0];
			for(int i=1; i<tValues.size(); i++)
				if(tValues[i] < temp) temp = tValues[i];
			return temp;
		}
	}

	virtual float getUnitHeight()
	{
		return 1.2; // the leg yScale is 1.0, the tabletop yScale is 0.2, so table height = 1.2 (cube height = 1)
	}

private:
	Box box;
};