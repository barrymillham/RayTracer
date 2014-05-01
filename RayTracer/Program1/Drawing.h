/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

// Used for storing and passing shader attribute/uniform handles (which are needed for drawing and buffering geometry objects, such as
// GeometryItem and Mesh)
struct AttribLocations
{
	unsigned v_pos;
	unsigned v_normal;
	unsigned u_proj;
	unsigned u_camera;
	unsigned u_model;
	unsigned u_lightPos;
	unsigned u_color;
	unsigned u_ambientOnly;
};

extern AttribLocations attribs;