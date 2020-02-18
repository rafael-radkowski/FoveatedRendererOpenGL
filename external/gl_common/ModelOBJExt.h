/*
ModelOBJExt - This code is part of CS/CPRE/ME 557 - Computer Graphics 

This class provides functions to load and render an 3D model stored as .obj file. 
In comparison to ModelOBJ, this extended class version also reads the material properties, if available. 
It also maintains its own model matrix as part of the class properties. 

The shader program still needs to be manually assigned.

Features:
- Load all meshes from an obj file. 
- Load all material from a file. 
- Automatically sets the materials to the shader program. 
- Draws the meshes (if a shader program was assigned)

Rafael Radkowski
Iowa State Univerity
rafael@iastate.edu
Sep. 12, 2018
rafael@iastate.edu

Last edit:
Dec 16, 2018, RR
	- fixed a bug that skipped severa triangles from being rendeered.
	- changed the code to process all data. 
*/

#ifndef OBJ_MODEL_EXT
#define OBJ_MODEL_EXT

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// local
#include "VertexBuffers.h"			// create vertex buffer object
#include "ShaderProgram.h"			// create a shader program
#include "CommonTypes.h"			// material and light types

#include "OBJLoader.h"


using namespace std;


namespace cs557
{

	class OBJModelExt {
	
	public:
		/*
		Load an OBJ model from file
		@param path_and_filename - number of rows
		@param shader_program - overwrite the default shader program by passing a hander to the constructor
		*/
		void create(string path_and_filename, int shader_program = -1);


		/*
		Draw the obj model
		@param viewMatrix - a view matrix object
		*/
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);


		/*
		Return the shader program
		@return - int containing the shader program
		*/
		int getProgram(void){return program;}


		// the object's  properties
		Properties 	prop;

	private:

		objl::Loader loader;

		int vaoID[1]; // Our Vertex Array Object
		int vboID[2]; // Our Vertex Buffer Object
		int iboID[1]; // Our Index  Object
		int program;
		int program_backup;

		int viewMatrixLocation;
		int modelMatrixLocation;
		int projMatrixLocation;

		int _N; // number of vertices
		int _I; // number indices
	};
}


#endif