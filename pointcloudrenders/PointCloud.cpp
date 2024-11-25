#include "PointCloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

PointCloud::PointCloud(std::string objFilename, GLfloat pointSize) 
	: pointSize(pointSize)
{
	ifstream inFile(objFilename);

	//For centering object
	float xmin, ymin, zmin = INFINITY;
	float xmax, ymax, zmax = -INFINITY;

	if (inFile.is_open()) { //File can be opened
		string line; //Current line in file

		while (getline(inFile, line)) { //Read each line in file
			if (line[0] == 'v' && line[1] == ' ') { //Specify vertex data only
				//Parse coordinate data
				stringstream ss(line);

				//Create point given data
				glm::vec3 point;

				ss.get(); //Skip 'v' specifier
				ss >> point.x >> point.y >> point.z;

				//Find max and min in each axis for centering object
				if (point.x < xmin) { xmin = point.x; }
				if (point.x > xmax) { xmax = point.x; }
				if (point.y < ymin) { ymin = point.y; }
				if (point.y > ymax) { ymax = point.y; }
				if (point.z < zmin) { zmin = point.z; }
				if (point.z > zmax) { zmax = point.z; }

				points.push_back(point);

			}
		}
	}
	else
	{
		cerr << "Can't open the file " << objFilename << endl;
	}
	
	//Calculate center for each axis using min/max values
	float xCenter = (xmax + xmin)/2;
	float yCenter = (ymax + ymin)/2;
	float zCenter = (zmax + zmin)/2;

	//Calculate the object dimensions for uniform scaling
	float xdim = xmax - xmin;
	float ydim = ymax - ymin;
	float zdim = zmax - zmin;

	//Use largest axis length to determine best fit
	float maxDim = fmax(fmax(xdim, ydim), zdim);

	//Universal value to fit "normalized" object into window

	float scaleFactor = 15;

	//Adjust every vertex to center object based on min/max values in each axis
	for (int i = 0; i < points.size(); i++) {

		//Center point by x
		if (xCenter < 0.0) { points[i].x += xCenter; }
		else { points[i].x -= xCenter; }
		//Center point by y
		if (yCenter < 0.0) { points[i].y += yCenter; }
		else { points[i].y -= yCenter; }
		//Center point by z
		if (zCenter < 0.0) { points[i].z += zCenter; }
		else { points[i].z -= zCenter; }

		//Uniformly scale object
		points[i].x = points[i].x * (1.0/maxDim) * scaleFactor;
		points[i].y = points[i].y * (1.0/maxDim) * scaleFactor;
		points[i].z = points[i].z * (1.0/maxDim) * scaleFactor;
	}

	// Set the model matrix to an identity matrix. 
	model = glm::mat4(1);

	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

PointCloud::~PointCloud() 
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void PointCloud::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader)
{
	// Actiavte the shader program 
	glUseProgram(shader);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

	// Bind the VAO
	glBindVertexArray(VAO);

	// Set point size
	glPointSize(pointSize);

	// Draw the points 
	glDrawArrays(GL_POINTS, 0, points.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void PointCloud::update()
{
	// Spin the cube by 1 degree
	spin(0.1f);
}

void PointCloud::updatePointSize(GLfloat size) 
{
	pointSize += size;
}

void PointCloud::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}
