/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#include "MyGLWidget.h"
#include "../glm/gtc/matrix_transform.hpp"

AttribLocations attribs;

MyGLWidget::MyGLWidget(QWidget* parent) : QGLWidget(parent) {
	iterator = 0;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::initializeGL() {
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();

	const char* vertexSource = textFileRead("lambert.vert");
	const char* fragmentSource = textFileRead("lambert.frag");
	glShaderSource(vertexShader, 1, &vertexSource, 0);
	glShaderSource(fragmentShader, 1, &fragmentSource, 0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Free malloc-allocated text buffers obtained from textFileRead() now that we no longer need them
	free((void*)vertexSource);
	free((void*)fragmentSource);

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	// Get the locations of VBO attributes and uniforms
	attribs.v_pos = glGetAttribLocation(shaderProgram, "vs_position");
	//unsigned int vColorLocation = glGetAttribLocation(shaderProgram, "vs_color");
	attribs.v_normal = glGetAttribLocation(shaderProgram, "vs_normal");
	attribs.u_proj = glGetUniformLocation(shaderProgram, "u_projMatrix");
	attribs.u_model = glGetUniformLocation(shaderProgram, "u_modelMatrix");
	attribs.u_camera = glGetUniformLocation(shaderProgram, "u_cameraMatrix");
	attribs.u_lightPos = glGetUniformLocation(shaderProgram, "u_lightPos");
	attribs.u_color = glGetUniformLocation(shaderProgram, "u_color");
	attribs.u_ambientOnly = glGetUniformLocation(shaderProgram, "u_ambientOnly");

	// **** TODO: have both GeometryItem::draw() and Mesh::draw() set the shader attribute pointers immediately before drawing
	// (to allow multiple meshes, and allow meshes to coexist with GeometryItems)

	// Initialize primitive geometry classes (GeometryItem derivatives)
	Box::staticInitialize(attribs);
	// Initialize geometry instances
	whiteBox.initialize(vec3(1,1,1)); // white
	greenBox.initialize(vec3(0,1,0)); // green
	yellowBox.initialize(vec3(1,1,0)); // yellow
	table.initialize(vec3(1,0,0)); // red
	chair.initialize(vec3(0,0,1)); // blue

	// Parse scene description and build scene graph
	parseSceneDescription(scene, "testScene.txt");

	// Read geometry description and buffer the mesh
	//parseGeometryDescription(mesh, "extrusion1.dat");

	// Initialize zoom, upDownAngle, and leftRightAngle
	zoom = 0;
	upDownAngle = 0;
	leftRightAngle = 0;
	// Initialize light position
	lightPos = vec4(0,10,0,1); // hovering over center of floor at y=+10
}

void MyGLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send the light source location to the shader
	glUniform4fv(attribs.u_lightPos, 1, &lightPos[0]);

	// Draw a small white box to represent the light source
	mat4 lightBoxTrans = glm::translate(mat4(1.0f), vec3(lightPos.x, lightPos.y, lightPos.z));
	mat4 lightBoxScale = glm::scale(mat4(1.0f), vec3(0.5, 0.5, 0.5));
	// Temporarily turn off advanced lighting so we can see the white box at full brightness
	glUniform1i(attribs.u_ambientOnly, 1);
	whiteBox.draw(lightBoxTrans * lightBoxScale);
	glUniform1i(attribs.u_ambientOnly, 0); // re-enable advanced lighting for the rest of the scene

	//mesh.draw(mat4(1.0f));
	scene.draw();

	glFlush();
}

void MyGLWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);

	projection = glm::perspective(90.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 30.0f);

	updateCamera();
}

// If this is not called from resizeGL(), paintGL() should be called immediately after.
void MyGLWidget::updateCamera()
{
	float zoomDistance = 20.0f - (float)zoom + 0.001; // 0.001 because this should never be actually zero
	//mat4 zoomTrans = glm::translate(mat4(1.0f), vec3(0,0,zoomDistance));
	mat4 rotate_x = glm::rotate(mat4(1.0f), (float)upDownAngle, vec3(1,0,0));
	mat4 rotate_y = glm::rotate(mat4(1.0f), (float)leftRightAngle, vec3(0,1,0));
	vec4 eyePos(0,0,zoomDistance,1);
	eyePos = rotate_y * rotate_x * eyePos;
	vec4 up(0,1,0,0);
	up = rotate_y * rotate_x * up;

	vec3 eyePos3d(eyePos.x, eyePos.y, eyePos.z);
	vec3 up3d(up.x, up.y, up.z);

	camera = glm::lookAt(eyePos3d, vec3(0,0,0), up3d);

	//mat4 proj_camera = projection * camera;
	//glUniformMatrix4fv(u_projLocation, 1, GL_FALSE, &proj_camera[0][0]);
	// Send the projection matrix by itself - we need to send camera as the front end of the model matrix chain
	// (so all our geometry is in camera space when we're dealing with lights).
	glUniformMatrix4fv(attribs.u_proj, 1, GL_FALSE, &projection[0][0]);
	
	// Send the camera matrix to the GPU as a uniform
	glUniformMatrix4fv(attribs.u_camera, 1, GL_FALSE, &camera[0][0]);
}

//from swiftless.com
// NOTE: returns a malloc-allocated buffer that must be freed by the caller
char* MyGLWidget::textFileRead(const char* fileName) {
    char* text;
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

void MyGLWidget::parseSceneDescription(SceneGraph &scene, std::string fileName)
{
	
	//The way stacking works: 
	/*
	The yScale of each piece of geometry is recorded as that geometry's height.
	That geometry is given to a SceneGraph Node which is then added as a child
		to furnitureRoot. 
	FurnitureRoot is a childNode of floor.
	Floor is a child of Scene, which is a member of MyGlWidget.

	//Drawing:
	Then, when you call scene.draw(), it calls head->draw() (Where head is the topmost node)
	That first transforms the same as it's parent's transformation, and then also transforms by
		the transformation that you pass into it in the Node() constructor.
	Then it calls draw() on its geometry. This draw function is virtual and routes to
		the draw function contained in the class that matches the type of geometry calling draw()
	That draw function will transform a box into whatever position it needs to be in in local 
		space and then call the most basic Draw() function (member of GeometryItem), which
		takes the VBO, NBO and IBO into account and sends all the information into the shader.

	*/

	
	
	// Clear the scene, in case there's already something there
	scene.clear();

	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(fileName);
		
		int floorXSize, floorZSize;
		int numItems;

		file >> floorXSize >> floorZSize >> numItems;

		// Start our tree at the floor
		/*mat4 scene_rotx = glm::rotate(mat4(1.0f), 0.0f, vec3(1.0f, 0.0f, 0.0f));
		mat4 scene_roty = glm::rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
		mat4 scene_rotz = glm::rotate(mat4(1.0f), 0.0f, vec3(0.0f, 0.0f, 1.0f));
		mat4 scene_scale = glm::scale(mat4(1.0f), vec3(2, 2, 2));
		mat4 floorScale = glm::scale(mat4(1.0f), vec3((float)floorXSize, 0.1f, (float)floorZSize));
		mat4 floorTransform = scene_rotz * scene_roty * scene_rotx * scene_scale * floorScale;*/
		//construct vectors to store transformation values in node
		SceneGraph::Node *floor = new SceneGraph::Node(&greenBox, vec3(0,0,0), vec3(0,0,0), vec3(2*(float)floorXSize,2*0.1f,2*(float)floorZSize));
		// But, we need to send up the camera matrix as a root node above the floor, so make the floor a child of that.
		//SceneGraph::Node *root = new SceneGraph::Node(0, camera);
		//scene.addChildToHead(root);
		//root->addChild(floor);
		scene.addChildToHead(floor);

		// Add a transformation node to reverse the floor's scaling, and place items on top of the floor
		// This will be the immediate parent for all floor-level furniture items.
		//floorTransform = glm::scale(floorTransform, vec3(2, 2, 2));
		//floorTransform = glm::inverse(floorScale) * floorTransform;
		mat4 onFloor_trans = glm::translate(mat4(1.0f), vec3(0,0.55f,0));
		//SceneGraph::Node *furnitureRoot = new SceneGraph::Node(0, glm::inverse(floorScale) * onFloor_trans);
		SceneGraph::Node *furnitureRoot = new SceneGraph::Node(0, vec3(0,0,0), vec3(0,0.55,0), vec3(1/((float)floorXSize), 1/(0.1f), 1/((float)floorZSize)));
		floor->addChild(furnitureRoot);


		// Add walls around the edges of the floor
		/*mat4 frontWall_scale = glm::scale(mat4(1.0f), vec3(floorXSize, WALL_HEIGHT, 0.1));
		mat4 frontWall_trans = glm::translate(mat4(1.0f), vec3(0, WALL_HEIGHT*.5, (floorZSize*.5)+0.05));
		furnitureRoot->addChild(new SceneGraph::Node(&box, frontWall_trans * frontWall_scale));
		mat4 backWall_trans = glm::translate(mat4(1.0f), vec3(0, WALL_HEIGHT*.5, -((floorZSize*.5)+0.05)));
		furnitureRoot->addChild(new SceneGraph::Node(&box, backWall_trans * frontWall_scale));
		mat4 leftWall_scale = glm::scale(mat4(1.0f), vec3(0.1f, WALL_HEIGHT, floorZSize));
		mat4 leftWall_trans = glm::translate(mat4(1.0f), vec3((floorXSize*.5f)+0.05, WALL_HEIGHT*.5f, 0));
		furnitureRoot->addChild(new SceneGraph::Node(&box, leftWall_trans * leftWall_scale));
		mat4 rightWall_trans = glm::translate(mat4(1.0f), vec3(-((floorXSize*.5f)+0.05), WALL_HEIGHT*.5f, 0));
		furnitureRoot->addChild(new SceneGraph::Node(&box, rightWall_trans * leftWall_scale));
*/
		// Read in the furniture from the file and build the scene graph
		// First off, we need to keep track of what the "stacking level" is at each grid location:
		float **stackingLevels = new float*[floorXSize];
		// Likewise, we're going to need another array to keep track of the "top" item at each grid location:
		SceneGraph::Node ***stackingItems = new SceneGraph::Node**[floorXSize];
		for(int i = 0; i < floorXSize; i++)
		{
			stackingLevels[i] = new float[floorZSize];
			stackingItems[i] = new SceneGraph::Node*[floorZSize];
		}
		// Initialize each grid location's stacking level and item pointer to 0
		for(int i = 0; i < floorXSize; i++)
		{
			for(int j = 0; j < floorZSize; j++)
			{
				stackingLevels[i][j] = 0;
				stackingItems[i][j] = 0; // null pointer means the grid location is empty
			}
		}

		// Build the scene
		for(int item = 0; item < numItems; item++)
		{
			std::string type;
			int xIndex, zIndex;
			float rotation;
			float xScale, yScale, zScale;

			file >> type >> xIndex >> zIndex >> rotation >> xScale >> yScale >> zScale;

			AbstractGeometryItem *geo;

			if(type == "box")
			{
				geo = &yellowBox;
				geo->setHeight(yScale); 
			}
			else if(type == "chair")
			{
				geo = &chair;
				geo->setHeight(yScale);
			}
			else if(type == "table")
			{
				geo = &table;
				geo->setHeight(yScale);
			}
			else
			{
				SceneGraphException ex;
				ex.reason = "SceneGraph: invalid furniture type \"" + type + "\"!";
				throw ex;
			}

			
			// For the translation, first we need to determine where this item's grid position is in world space.
			// For simplicity, we'll define our grid to be a floorXSize-by-floorZSize "square" in the x-z plane, centered
			// at the origin. The grid point 0,0 will be at (-floorXSize*.5f, -floorZSize*.5f) in world space.
			//mat4 trans_grid = glm::translate(mat4(1.0f), vec3(-((float)floorXSize)/2.0f + xIndex, 0.0f, -((float)floorZSize)/2.0f + zIndex));
			//// Now, we need to position the item vertically, on top of any previous items in its grid location.
			//mat4 trans_y = glm::translate(mat4(1.0f), vec3(0.0f, stackingLevels[xIndex][zIndex], 0.0f)); // 0.5 is a hardcoded hack, specific to current furniture - need to change this
			//// Rotate and scale
			//mat4 trans_rot = glm::rotate(mat4(1.0f), rotation, vec3(0.0f, 1.0f, 0.0f)); // rotation is about the y-axis
			//mat4 trans_scale = glm::scale(mat4(1.0f), vec3(xScale, yScale, zScale));
			vec3 gridTranslation(-((float)floorXSize)/2.0f + xIndex, 0.5f, -((float)floorZSize)/2.0f + zIndex);
			
			//stackingLevels[xIndex][zIndex] += itemHeight;
			stackingLevels[xIndex][zIndex] = 0.0f;

			SceneGraph::Node *thisItem = 0;

			// Add the furniture item as a child node of whatever's under it on its grid location.
			// If there's nothing under it, we will make it a child of the furniture root.
			//SceneGraph::Node *thisItem = new SceneGraph::Node(geo, trans);
			if(stackingItems[xIndex][zIndex] == 0)
			{
				//mat4 trans = trans_y * trans_grid  * trans_rot * trans_scale;
				thisItem = new SceneGraph::Node(geo, vec3(0,rotation,0), gridTranslation, vec3(xScale, yScale, zScale));
				furnitureRoot->addChild(thisItem);
			}
			else
			{
				//mat4 trans = trans_y * trans_rot * trans_scale;
				thisItem = new SceneGraph::Node(geo, vec3(0,rotation,0), vec3(0,0,0), vec3(xScale, yScale, zScale));
				stackingItems[xIndex][zIndex]->addChild(thisItem);
			}
			stackingItems[xIndex][zIndex] = thisItem;

			
			//as of right now, i'm only having "furniture roots" be selectable until we get stacking working
			objects.push_back(thisItem);
		}

		// Free dynamically allocated memory for grid arrays
		for(int i = 0; i < floorXSize; i++)
		{
			delete [] stackingLevels[i];
			delete [] stackingItems[i];
		}
		delete [] stackingLevels;
		delete [] stackingItems;

		//set the first vector as the default "selected"
		iterator = 0;
		objects[iterator]->setSelected(true);
		//SET INITIAL SLIDER VALUES
		float temp = objects[iterator]->getScalingX();
		if(temp > 100) temp = 100;
		emit changeScalingXSliderValue(temp*100.0f);
		temp = objects[iterator]->getScalingY();
		if(temp > 100) temp = 100;
		emit changeScalingYSliderValue(temp*100.0f);
		temp = objects[iterator]->getScalingZ();
		if(temp > 100) temp = 100;
		emit changeScalingZSliderValue(temp*100.0f);

	}
	catch(std::ifstream::failure)
	{
		SceneGraphException ex;
		ex.reason = "SceneGraph: file error while parsing scene description!";
		throw ex;
	}
}

void MyGLWidget::changeZoom(int zoomLevel)
{
	zoom = zoomLevel; // integer in range [1,100]
	updateCamera();
	update();
}

void MyGLWidget::changeUpDownAngle(int angle)
{
	upDownAngle = -angle;
	updateCamera();
	update();
}

void MyGLWidget::changeLeftRightAngle(int angle)
{
	leftRightAngle = -angle;
	updateCamera();
	update();
}

void MyGLWidget::loadNewScene(QString text)
{
	parseSceneDescription(scene, text.toStdString());
	//parseGeometryDescription(mesh, text.toStdString());
	zoom = 0;
	upDownAngle = 0;
	leftRightAngle = 0;
	updateCamera();
	update();
}

void MyGLWidget::lightXInc()
{
	lightPos.x += 1;
	update();
}

void MyGLWidget::lightXDec()
{
	lightPos.x -= 1;
	update();
}

void MyGLWidget::lightYInc()
{
	lightPos.y += 1;
	update();
}

void MyGLWidget::lightYDec()
{
	lightPos.y -= 1;
	update();
}

void MyGLWidget::lightZInc()
{
	lightPos.z += 1;
	update();
}

void MyGLWidget::lightZDec()
{
	lightPos.z -= 1;
	update();
}

void MyGLWidget::nextObject()
{
	objects[iterator]->setSelected(false);
	iterator++;
	if(iterator > objects.size()-1) iterator = 0;
	objects[iterator]->setSelected(true);
	emit changeRotationSliderValue(objects[iterator]->getRotationDegreesY()-180);

	//MAKE SURE TO CLAMP TO 100 for slider
	float temp = objects[iterator]->getScalingX();
	if(temp > 100) temp = 100;
	emit changeScalingXSliderValue(temp*100.0f);
	temp = objects[iterator]->getScalingY();
	if(temp > 100) temp = 100;
	emit changeScalingYSliderValue(temp*100.0f);
	temp = objects[iterator]->getScalingZ();
	if(temp > 100) temp = 100;
	emit changeScalingZSliderValue(temp*100.0f);

	repaint();
}

void MyGLWidget::previousObject()
{
	objects[iterator]->setSelected(false);
	iterator--;
	if(iterator < 0)
		iterator = objects.size()-1;
	objects[iterator]->setSelected(true);
	emit changeRotationSliderValue(objects[iterator]->getRotationDegreesY()-180);
	
	//MAKE SURE TO CLAMP TO 100 for slider
	float temp = objects[iterator]->getScalingX();
	if(temp > 100) temp = 100;
	emit changeScalingXSliderValue(temp*100.0f);
	temp = objects[iterator]->getScalingY();
	if(temp > 100) temp = 100;
	emit changeScalingYSliderValue(temp*100.0f);
	temp = objects[iterator]->getScalingZ();
	if(temp > 100) temp = 100;
	emit changeScalingZSliderValue(temp*100.0f);

	repaint();
}

void MyGLWidget::changeRotationDegrees(int r)
{
	objects[iterator]->setRotationDegreesY(r+180);
	update();
}

void MyGLWidget::changeScalingX(int s)
{
	objects[iterator]->setScalingX(s/100.0f);
	update();
}
void MyGLWidget::changeScalingY(int s)
{
	objects[iterator]->setScalingY(s/100.0f);
	update();
}
void MyGLWidget::changeScalingZ(int s)
{
	objects[iterator]->setScalingZ(s/100.0f);
	update();
}

void MyGLWidget::plusTranslationX()
{
	float temp = objects[iterator]->getTranslationX();
	objects[iterator]->setTranslationX(temp+1.0f);
	update();
}
void MyGLWidget::minusTranslationX()
{
	float temp = objects[iterator]->getTranslationX();
	objects[iterator]->setTranslationX(temp-1.0f);
	update();
}
void MyGLWidget::plusTranslationY()
{
	float temp = objects[iterator]->getTranslationY();
	objects[iterator]->setTranslationY(temp+1.0f);
	update();
}
void MyGLWidget::minusTranslationY()
{
	float temp = objects[iterator]->getTranslationY();
	objects[iterator]->setTranslationY(temp-1.0f);
	update();
}
void MyGLWidget::plusTranslationZ()
{
	float temp = objects[iterator]->getTranslationZ();
	objects[iterator]->setTranslationZ(temp+1.0f);
	update();
}
void MyGLWidget::minusTranslationZ()
{
	float temp = objects[iterator]->getTranslationZ();
	objects[iterator]->setTranslationZ(temp-1.0f);
	update();
}


void MyGLWidget::parseGeometryDescription(Mesh &mesh, std::string filename)
{
	mesh.clear();

	// Read in the polygon description from file
	std::ifstream inputFile(filename);

	std::string procedureType;
	inputFile >> procedureType;

	if(procedureType == "extrusion")
	{
		float height;
		int numPoints;

		inputFile >> height >> numPoints;

		std::vector<vec3> polygonPoints;
		for(int i = 0; i < numPoints - 1; i++) // Note: loops one less time to skip repeated first point at end of input
		{
			vec3 loc, normal(0,1,0);
		
			inputFile >> loc.x >> loc.z;
			loc.y = 0.0f;

			polygonPoints.push_back(loc);
		}

		// Test for convexity; if the polygon is convex, build an index buffer based on its triangulation
		if(polygonPoints.size() < 3)
			return; // our algorithm won't work unless there are at least 3 points - there should be, if the input was correct

		bool isConvex = true;
		// Find vertex 0's normal
		vec3 n = glm::cross(polygonPoints[1] - polygonPoints[0], polygonPoints[polygonPoints.size()-1] - polygonPoints[0]);
		// Find the normal for each of the other vertices; if it's opposite v0's, the polygon is non-convex.
		// While we're at it, add up the number of points with positive and negative normals (in the y dimension) -
		// that'll tell us the winding order by which the polygon was specified in the input file.
		int numPositiveNormals = 0;
		for(int i = 1; i < polygonPoints.size(); i++)
		{
			int next = (i+1) % polygonPoints.size();
			vec3 t = glm::cross(polygonPoints[next] - polygonPoints[i], polygonPoints[i-1] - polygonPoints[i]);
			vec3 x = glm::normalize(t);
			vec3 y = glm::normalize(-n);
			if(x == y)
				isConvex = false;
			if(x.y == 1)
				numPositiveNormals++;
		}

		if(isConvex)
			isConvex = true;
		else
			isConvex = false;

		// If convex, triangulate the polygon into faces and add it to the mesh (twice, once for each endcap)
		if(isConvex)
		{
			for(int i = 1; i < polygonPoints.size() - 1; i++)
			{
				Mesh::Face face;
				face.p0 = polygonPoints[0];
				face.p1 = polygonPoints[i];
				face.p2 = polygonPoints[(i+1) % polygonPoints.size()];
				face.normal = vec3(0,-1,0);

				mesh.addFace(face);
				
				face.p0.y += height;
				face.p1.y += height;
				face.p2.y += height;
				face.normal.y = 1;

				mesh.addFace(face);
			}
		}

		// Triangulate the sides into faces and add them to the mesh
		for(int i = 0; i < polygonPoints.size(); i++)
		{
			Mesh::Face face;

			// First triangle: bottom[i], top[i], top[i+1]
			face.p0 = face.p1 = polygonPoints[i];
			face.p1.y += height;
			face.p2 = polygonPoints[(i+1) % polygonPoints.size()];
			face.p2.y += height;
			if(numPositiveNormals > polygonPoints.size() - numPositiveNormals)
				face.normal = glm::normalize(glm::cross(face.p0 - face.p1, face.p2 - face.p1));
			else
				face.normal = glm::normalize(glm::cross(face.p2 - face.p1, face.p0 - face.p1));
			mesh.addFace(face);

			Mesh::Face face2;

			// Second triangle: top[i+1], bottom[i+1], bottom[i]
			face2.p0 = face2.p1 = polygonPoints[(i+1) % polygonPoints.size()];
			face2.p0.y += height;
			face2.p2 = polygonPoints[i];
			if(numPositiveNormals > polygonPoints.size() - numPositiveNormals)
				face2.normal = glm::normalize(glm::cross(face2.p0 - face2.p1, face2.p2 - face2.p1));
			else
				face2.normal = glm::normalize(glm::cross(face2.p2 - face2.p1, face2.p0 - face2.p1));
			mesh.addFace(face2);
		}
	}
	else if(procedureType == "surfrev")
	{
		int numSlices;
		int numPoints;

		inputFile >> numSlices >> numPoints;

		// numSlices must be at least 3, or the mesh will be flat - we don't want this
		if(numSlices < 3)
			return;

		std::vector<vec3> polygonPoints;
		for(int i = 0; i < numPoints; i++)
		{
			vec3 loc, normal(0,1,0);
		
			inputFile >> loc.x >> loc.y;
			loc.z = 0.0f;

			// x-coordinates may not be negative - clamp to [0, inf.)
			if(loc.x < 0)
				loc.x = 0;

			polygonPoints.push_back(loc);
		}

		// Check if the polyline ends at the y-axis (on both ends) - if not, we'll need to add additional point(s) with x=0 to cap it off
		if(polygonPoints[0].x != 0.0f)
			polygonPoints.insert(polygonPoints.begin(), vec3(0, polygonPoints[0].y, 0));
		if(polygonPoints[polygonPoints.size() - 1].x != 0.0f)
			polygonPoints.push_back(vec3(0, polygonPoints[polygonPoints.size() - 1].y, 0));

		// Perform the rotation
		int sliceSize = 360 / numSlices;
		for(int i = 0; i < 360; i += sliceSize)
		{
			// If this is the last slice, make sure we don't overshoot past 360 degrees
			// Note: the "yes" case was erroneously "i - 360" when I turned this in; it didn't cause problems for the two sample cases
			// included with the assignment, but will cause problems in the general case.
			int thisSlice = (i + sliceSize > 360) ? 360 - i : sliceSize;

			// Determine the rotated points on the polyline for the beginning and end of this slice
			std::vector<vec4> begin, end;
			for(int j = 0; j < polygonPoints.size(); j++)
			{
				// These were wrong in the turned-in version too - I had i*sliceSize instead of i in the next two lines
				begin.push_back(glm::rotate(mat4(1.0f), (float)(i), vec3(0,1,0)) * vec4(polygonPoints[j], 1));
				end.push_back(glm::rotate(mat4(1.0f), (float)(i + thisSlice), vec3(0,1,0)) * vec4(polygonPoints[j], 1));
			}

			// Triangulate the side of this slice and add the resultant faces to the mesh
			for(int j = 0; j < polygonPoints.size() - 1; j++)
			{
				Mesh::Face face;

				// First triangle
				face.p0 = vec3(begin[j]);
				face.p1 = vec3(begin[j+1]);
				face.p2 = vec3(end[j+1]);
				face.normal = glm::normalize(glm::cross(face.p0 - face.p1, face.p2 - face.p1));
				mesh.addFace(face);

				// Second triangle
				face.p0 = vec3(end[j+1]);
				face.p1 = vec3(end[j]);
				face.p2 = vec3(begin[j]);
				face.normal = glm::normalize(glm::cross(face.p0 - face.p1, face.p2 - face.p1));
				mesh.addFace(face);
			}
		}
	}

	mesh.bufferData(attribs);
}