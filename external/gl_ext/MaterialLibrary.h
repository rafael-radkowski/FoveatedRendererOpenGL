#pragma once


// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <chrono>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// glfw includes
#include <GLFW/glfw3.h>


// local
#include "FileUtils.h"				// checks whether the model file is at its given location.
#include "CommonTypes.h"			// material
#include "Texture2D.h"
#include "TextureLoader.h"


namespace cs557{


class MaterialLibrary
{
public:

	/*
	Add a material to the library. The material is only added if a 
	material with a similar name does not exists, or the user set the parameter
	'allowsCopies' to true. The material is renamed in this case. 
	@param material - a struct containing material information including a name
	@param allowCopies - set true, to allow material copies. 
	@param materialIdx - a location to store the material id. 
	@return true, if the material was successfully added. 
	*/
	static bool AddMaterial(const Material material, int& materialIdx, const bool allowCopies = false);


	/*
	Return a material 
	@param materialId -  the material id
	@param material - reference to the material or a default matieral if the request one does not exist. 
	*/
	static bool GetMaterial(const int materialId, Material& material);


	/*
	Return a material  by name
	@param material_name - string containing the material name;
	@param material - reference to the material or a default matieral if the request one does not exist. 
	*/
	static bool GetMaterial(const std::string material_name, Material& material);


	/*
	This enables the glsl material. 
	The function overwrites the material index so that the shader program reads
	the material from the right material slot. 
	@param glsl_material_id - the glsl material library id number
	*/
	static bool EnableMaterial(const int program, const int glsl_material_id);



	/*
	Copies the material to a program
	@param program - integer id with the glsl shader program
	@param material_id - the material index. 
	*/
	static bool CopyMaterialToGPU(const int program, const int material_id);


	/*
	Copy the texture to the gpu.
	@param program - integer id with the glsl shader program
	@param material_id - the texture index. 
	*/
	static bool CopyTextureToGPU(const int program, const int texture_id);


private:


	/*
	Search for a material by name. 
	@param name - string with the material name.
	@return int containing the material id or -1 if the material does not exist. 
	*/
	static int Search(std::string name);


	/*
	Determine whether the texture already exists. 
	*/
	static int SearchTex(std::string name);

	/*
	Process and load the texture maps
	*/
	static bool ProcessTextures(Material& material);

};


};//namespace cs557{