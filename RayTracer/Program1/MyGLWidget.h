/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once
#define GLEW_STATIC
#include "glew.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include <QGLWidget>
#include <QLineEdit>
#include <stdlib.h>

#include <string>
#include <fstream>
#include <vector>

#include "Drawing.h"

#include "SceneGraph.h"
#include "Box.h"
#include "Table.h"
#include "Chair.h"

#include "Mesh.h"

using glm::vec3;
using glm::vec4;

#define WALL_HEIGHT 3.0f

class MyGLWidget : public QGLWidget
{
	Q_OBJECT
public:
	MyGLWidget(QWidget*);
	~MyGLWidget(void);

	void initializeGL(void);
	void paintGL(void);
	void resizeGL(int, int);

	//helper function to read shader source and put it in a char array
	//thanks to Swiftless.com
	char* textFileRead(const char*);

public slots:
	void changeZoom(int);
	void changeUpDownAngle(int);
	void changeLeftRightAngle(int);
	void loadNewScene(QString);
	void lightXInc();
	void lightXDec();
	void lightYInc();
	void lightYDec();
	void lightZInc();
	void lightZDec();
	void nextObject();
	void previousObject();

private:
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;

	//AttribLocations attribs; // now a global variable, declared as an extern in Drawing.h and defined in MyGLWidget.cpp

	Mesh mesh;

	std::vector<SceneGraph::Node*> objects;
	int iterator;

	SceneGraph scene;
	Box whiteBox, greenBox, yellowBox;
	Table table;
	Chair chair;

	glm::mat4 projection, camera;

	int zoom, upDownAngle, leftRightAngle;
	vec4 lightPos;

	void updateCamera();
	void parseSceneDescription(SceneGraph &scene, std::string fileName);

	//void parseGeometryDescription(Mesh &mesh, std::string fileName);
};