/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#include <exception>
#include <string>

class SceneGraphException : public std::exception
{
	virtual const char* what() const
	{
		return reason.c_str();
	}

public:
	std::string reason;
};

class MeshException : public std::exception
{
	virtual const char* what() const
	{
		return reason.c_str();
	}

public:
	std::string reason;
};