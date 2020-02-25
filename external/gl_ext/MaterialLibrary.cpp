#include "MaterialLibrary.h"


namespace ns_MaterialLibrary
{
	
	std::vector<cs557::Material>		library;


	std::vector<cs557::Texture>			tex_library;
	

	cs557::Material						default_material;

	// the material index counter counts the current available material location.
	// The glsl struct has 100 material slots. 
	int									glsl_material_index_counter = 10; // keep the 10 first materials to be modified by a user. 
	int									glsl_max_material = 100;


	int									glsl_texture_index_counter = 0; // keep the 10 first materials to be modified by a user. 
	int									glsl_max_texture = 50;

	int									glsl_texture_unit = 0;

};

using namespace cs557;
using namespace ns_MaterialLibrary;

/*
Add a material to the library. The material is only added if a 
material with a similar name does not exists, or the user set the parameter
'allowsCopies' to true. The material is renamed in this case. 
@param material - a struct containing material information including a name
@param allowCopies - set true, to allow material copies. 
@param materialIdx - a location to store the material id. 
@return true, if the material was successfully added. 
*/
//static 
bool MaterialLibrary::AddMaterial(const Material material, int& materialIdx, const bool allowCopies )
{
	int id = Search(material.name);

	if (id != -1) {
		// Material exists
		materialIdx = id;
		return true;
	}


	// material does not exists
	library.push_back(material);
	library.back().material_index = glsl_material_index_counter;
	
	materialIdx = library.size()-1;

	// process the textures
	ProcessTextures(library[library.size()-1]);

	// increase the counter
	glsl_material_index_counter++;

	return true;
}


/*
Return a material 
@param materialId -  the material id
@param material - reference to the material or a default matieral if the request one does not exist. 
*/
//static 
bool MaterialLibrary::GetMaterial(const int materialId, Material& material)
{
	if (materialId >= library.size() || materialId < 0) {
		material = default_material;
		return false;
	}

	material = library[materialId];

	return true;
}


/*
Return a material  by name
@param material_name - string containing the material name;
@param material - reference to the material or a default matieral if the request one does not exist. 
*/
//static 
bool MaterialLibrary::GetMaterial(const std::string material_name, Material& material)
{
	int id = Search(material_name);

	if (id == -1) {
		material = default_material;
		return false;
	}

	material = library[id];

	return true;
}


/*
Search for a material by name. 
@param name - string with the material name.
@return int containing the material id or -1 if the material does not exist. 
*/
//static 
int  MaterialLibrary::Search(std::string name)
{
	for (int i = 0; i < library.size(); i++) {
		if (library[i].name.compare(name) == 0) {
			return i;
		}
	}
	return -1;
}


/*
This enables the glsl material. 
The function overwrites the material index so that the shader program reads
the material from the right material slot. 
*/
//static 
bool MaterialLibrary::EnableMaterial(const int program, const int glsl_material_id)
{
	//glUseProgram(program);
	//glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "material_index" ), library[glsl_material_id].material_index);
	//glUseProgram(program);

	

	glUseProgram(program);

	int tex_id = library[glsl_material_id].texture_id;
	if ( tex_id != -1) {

		glEnable(GL_TEXTURE_2D);
		tex_library[tex_id].setActive(program);

		//glUseProgram(program);

		//// Activate the texture unit and bind the texture. 
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, tex_library[tex_id].tex_id);
		//
		//// Fetch the texture location and set the parameter to 0.
		//// Note that 0 is the number of the texture unit GL_TEXTURE0.
		//int location = glGetUniformLocation(program, "tex[0].tex_kd");
		//glUniform1i(location, 0);

		//// set the texture to active. 
		//int u = glGetUniformLocation(program, "tex[0].with_tex_kd");
		//glUniform1i(u, 1);


		//glUseProgram(program);
	}
	else
	{
		//int u = glGetUniformLocation(program, "tex[0].with_tex_kd");
		//glUniform1i(u, 0);

		//glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		int mu = glGetUniformLocation(program, "texture_multiplier");
		glUniform1f(mu, 0.0);
	}

	return true;
}


/*
Copies the material to a program
@param program - integer id with the glsl shader program
@param material_id - the material index. 
*/
//static 
bool MaterialLibrary::CopyMaterialToGPU(const int program, const int material_id)
{
	if (material_id >= library.size() || material_id < 0) {
		return false;
	}

	library[material_id].apply(program);


	//glUseProgram(program);
	// set the texture to active. 
	//int u = glGetUniformLocation(program, "tex[0].with_tex_kd");
	//glUniform1i(u, 0);
	//glUseProgram(program);


	// activate textures if available
	CopyTextureToGPU( program, library[material_id].texture_id);


	return true;
}



/*
Copy the texture to the gpu.
@param program - integer id with the glsl shader program
@param material_id - the texture index. 
*/
///static 
bool MaterialLibrary::CopyTextureToGPU(const int program, const int texture_id)
{
	if (texture_id >= tex_library.size() || texture_id < 0) {
		glUseProgram(program);
		// set the texture to active. 
		int u = glGetUniformLocation(program, "tex[0].with_tex_kd");
		glUniform1i(u, 0);
		u = glGetUniformLocation(program, "texture_multiplier");
		glUniform1f(u, 0.0);
		glUseProgram(program);
		return false;
	}

	


	tex_library[texture_id].apply(program);

}



/*
Determine whether the texture already exists. 
*/
//static 
int MaterialLibrary::SearchTex(std::string name)
{
	for (int i = 0; i < tex_library.size(); i++) {
		if (tex_library[i].name.compare(name) == 0) {
			return i;
		}
	}
	return -1;
}


/*
Process and load the texture maps
*/
//static 
bool MaterialLibrary::ProcessTextures(Material& material)
{
	
	//-------------------------------------------------------------------------------------------------------
	// diffuse texture
	string tex_Kd = material.map_diffuse;

	
	
	if (tex_Kd.length() > 0) {

		int tex_id = SearchTex(tex_Kd);

		if(tex_id == -1){

	
			GLint max_tex[1];
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, max_tex);

			cs557::Texture	tex;

			string tex_path;
			tex_path.append(tex_Kd);

			// Load the texture
			TextureLoader::Load(tex_path, &tex.map, &tex.width, &tex.height, &tex.channels);

			// create the gl texture
			//glUseProgram(program);
			cs557::CreateTexture2D(tex.width, tex.height, tex.channels, tex.map, &tex.tex_id, GL_REPEAT, GL_TEXTURE0);

			// Activate the texture unit and bind the texture. 
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex.tex_id);

			tex.type = DIFFUSE;
			tex.name = tex_Kd;
			tex.texture_index = glsl_texture_index_counter;
			glsl_texture_index_counter++;

			tex.tex_unit = GL_TEXTURE0;
			

			cout << "bind texture " << tex_Kd << " to texture id " << tex.tex_id << ", material index " << tex.texture_index << endl;
			glsl_texture_unit++;
			/*
			// Fetch the texture location and set the parameter to 0.
			// Note that 0 is the number of the texture unit GL_TEXTURE0.
			tex.diff.tex_loc = glGetUniformLocation(program, "tex[0].tex_kd");
			glUniform1i(tex.diff.tex_loc, 0);

			// set the texture to active. 
			int u = glGetUniformLocation(program, "tex[0].with_tex_kd");
			glUniform1i(u, 1);

			glUseProgram(0);
			*/



			// store the texture
			tex_library.push_back(tex);
			material.texture_id = tex_library.size() - 1;

		}else
		{
			material.texture_id = tex_id;
		}
	}


	
	return true;
}
