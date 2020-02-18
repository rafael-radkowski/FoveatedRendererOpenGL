#pragma once



// stl include
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

// glfw includes
#include <GLFW/glfw3.h>

// local
#define GLM_ENABLE_EXPERIMENTAL
#include "Window.h" // the windows
#include "OpenGLDefaults.h" // some open gl and glew defaults
#include "VertexBuffers.h"  // create vertex buffer object
#include "ShaderProgram.h"  // create a shader program
#include "CommonTypes.h"  // types that all projects use
#include "RenderToTexture.h" // for the prerender texture

// Geometry
#include "ModelCoordinateSystem.h"
#include "ModelPlane.h"
#include "ModelObj.h"
#include "ModelOBJExt.h"
#include "ModelSphere.h"




typedef struct GridParams
{
	int		rows; // z
	int		cols;  //x 
	int		depth; // y
	float	space_rows;
	float	space_cols;
	float	space_depth;


	std::vector<glm::vec3>	position;

	glm::vec3	start;

	GridParams() {
		rows = 2;
		cols = 2;
		depth = 1;
		space_rows = 1.0;
		space_cols = 1.0;
		space_depth = 1.0;
	}


}GridParams;




class DefaultModels {

public:

	/*
	Create a plane
	*/
	void createPlane(float sizeX, float sizeY, unsigned int program);

	/*
	Draw all models 
	*/
	void draw(glm::mat4 pm, glm::mat4 vm);

	/*
	Load a 3D model from an obj file
	*/
	bool loadModel(std::string path_and_file, int shader_program,  glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f) );



	bool createModelGrid(std::string path_and_file, int grid_rows, int grid_columns,  int grid_depth, float rows_spacing, float col_spacing, float depth_spacing,
						int shader_program, glm::vec3 start_point = glm::vec3(0.0f, 0.0f, 0.0f) );

private:

	// a plane
	cs557::Plane		plane0;
	cs557::Material		plane0_mat;

	std::vector<cs557::OBJModel> _scene_models;

	// models that will be rendererd as a grid
	std::vector<cs557::OBJModel> _grid_models;
	std::vector<GridParams>		 _grid_params;

};