/*
ModelOBJ - This code is part of CS/CPRE/ME 557 - Computer Graphics 

This class provides functions to load and render an 3D model stored as .obj file. 
However, it just loads the meshes from an obj file and renders them. 
Materials and a shader program must be manually assigned. 

Features:
- Load all meshes from an obj file. 
- Draws the meshes (if a shader program was assigned)

Rafael Radkowski
Iowa State Univerity
rafael@iastate.edu
Sep. 12, 2018
rafael@iastate.edu

Last edit:
Dec 16, 2018, RR
	- fixed a bug that skipped several triangles from being rendeered.
	- changed the code to process all data. 

Oct 20, 2019, RR
	- Removed some legacy code. 
	- Test whether points have been loaded before rendering. 
	- Added the class FileUtils to the code to check whether the model is at its indicated location. 

Dec 20, 2019, RR
	- Added an draw api into the class that comes without a model matrix. This adaptation is necessary for SurfExtract.
	- Added the namespace cs557 to the FileUtils function to prevent conflicts with other libraries. 
	- Added function to process more meshes

Feb 16, 2020, RR
	- Added material to the laoder. 
	- Added a texture loader (texture is not processed yet).
*/
#pragma once
#include "OBJLoader.h"

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <chrono>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// local
#include "VertexBuffers.h"			// create vertex buffer object
#include "ShaderProgram.h"			// create a shader program
#include "FileUtils.h"				// checks whether the model file is at its given location.
#include "CommonTypes.h"			// material
#include "Texture2D.h"
#include "TextureLoader.h"
#include "MaterialLibrary.h"		// manages the materials

using namespace std;


namespace cs557
{

	class OBJModel {

	private:


		// Structure: Vertex
		//
		// Description: Model Vertex object that holds
		//	a Position, Normal, and Texture Coordinate
		typedef struct Vertex
		{
			// Position Vector
			glm::vec3 position;

			// Normal Vector
			glm::vec3 normal;

			// Texture Coordinate Vector
			glm::vec2 textureCoordinate;

		}Vertex;

	
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
		@param modelMatrix - a model matrix object.
		*/
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix);
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

		/*
		Return the shader program
		@return - int containing the shader program
		*/
		int getProgram(void){return program;}
		
				/*
		Set the object's model matrix
		@param m - 4x4 model matrix
		*/
		void setModelMatrix(glm::mat4 m) {_prop.model_matrix = m; }


		/*
		Set the texture parameters.
		@param blend_mode - set the texture blend mode. MODULATE and REPLACE are currently supported. 
		*/
		void OBJModel::setTextureParam(TextureMode blend_mode);


		/*
		Enable or disable texture rendering. 
		@param value - true to enable texture rendering. 
		*/
		void enableTextureRendering(bool value);





	private:

		/*
		Process the diffuse, ambient, and specular texture of the object. 
		The function loads the textures from a file (using OpenCV), creates the texture object, 
		and copies the textures to the gpu.
		@param program - the shader program for this object. 
		@param curMesh - the current mesh that this instance creates.
		@param path - the path and file from which this object gets loaded. The function extracts the path.
		*/
		void processTextures(int& program, objl::Mesh& curMesh, string path);


		int vaoID[1]; // Our Vertex Array Object
		int vboID[2]; // Our Vertex Buffer Object
		int iboID[1]; // Our Index  Object
		int program;
		int program_backup;

		int viewMatrixLocation;
		int modelMatrixLocation;
		int projMatrixLocation;

		bool	with_textures; // to enable or disable texture rendering

		// the object's  properties
		Properties 	_prop;


		// indices to render
		std::vector<int>		start_index;
		std::vector<int>		length;

		// materials, their ids in the material library. 
		// the materials are stored per mesh.
		std::vector<int>					materials_ids;
		std::vector< cs557::TexMaterial>	textures;// textures per mesh
	
		int _N; // number of vertices
		int _I; // number indices
	};
}