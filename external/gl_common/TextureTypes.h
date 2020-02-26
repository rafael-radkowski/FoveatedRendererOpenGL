#pragma once


// stl include
#include <iostream>
#include <string>
#include <strstream>
#include <sstream>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions



namespace cs557{


typedef enum {
	DISABLED,
	REPLACE,	//C = Ct
	MODULATE, //	C = Ct*Cf
	DECAL //	C = Cf * (1 – At) + Ct * At

}TextureMode;

typedef enum {
	DIFFUSE,
	SPECULAR,	//C = Ct
	AMBIENT, //	C = Ct*Cf
	EMISSIVE
}TextureType;


/*
Datatype for a single textures.
Stores all relevant texture data and meta data. 
*/
typedef struct _Texture {

	unsigned int		tex_unit; // texture unit
	unsigned int		tex_id; // the glsl texture id
	int					tex_loc; // texture glsl shader location on host system
	unsigned char*		map; // location of the texture data
	int					width; // texture height 
	int					height; // texture width
	int					channels; // texture channels
	std::string			name; // the texture name;

	bool	with_error_check;

	int					texture_index; // the texture index of this material in the glsl texture lib
	float				texture_multiplier;


	TextureType			type;

	_Texture() {
		tex_unit = 0;
		tex_id = 0;
		tex_loc = -1;
		map = 0;
		width = 0;
		height = 0;
		channels = 0;
		name = "Texture";
		type = DIFFUSE;
		texture_index = 0;
		texture_multiplier = 0.8;

		with_error_check = true;
	
	}


	inline void setActive(int shader_program_id) {

		glUseProgram(shader_program_id);

		// Activate the texture unit and bind the texture. 
		glActiveTexture(tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex_id);

	
		

		if(type == DIFFUSE){
			
			if(checkName(shader_program_id, "texture_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_index" ), texture_index);

			// Fetch the texture location and set the parameter to 0.
			// Note that 0 is the number of the texture unit GL_TEXTURE0.
			int location = glGetUniformLocation(shader_program_id, getVariableName("tex", texture_index, "tex_kd").c_str());
			glUniform1i(location, tex_unit);

			// set the texture to active. 
			int u = glGetUniformLocation(shader_program_id, getVariableName("tex", texture_index, "with_tex_kd").c_str());
			glUniform1i(u, 1);

			if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), texture_multiplier);
		}
		else if(type == AMBIENT){

			if(checkName(shader_program_id, "texture_ka_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_ka_index" ), texture_index);
			
			// Fetch the texture location and set the parameter to 0.
			// Note that 0 is the number of the texture unit GL_TEXTURE0.
			int location = glGetUniformLocation(shader_program_id, getVariableName("tex", texture_index, "tex_ka").c_str());
			glUniform1i(location, tex_unit);

			// set the texture to active. 
			int u = glGetUniformLocation(shader_program_id, getVariableName("tex", texture_index, "with_tex_ka").c_str());
			glUniform1i(u, 1);

			if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), texture_multiplier);

		}

		glUseProgram(shader_program_id);
	}
	

	inline void apply(int shader_program_id)
    {
		glUseProgram(shader_program_id );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_id);

		if(checkName(shader_program_id, "texture_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_index" ), texture_index);
		if(checkName(shader_program_id, "texture_ka_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_ka_index" ), texture_index);
		if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), texture_multiplier);
		
        if(checkName(shader_program_id, getVariableName("tex", texture_index, "tex_kd") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "tex_kd").c_str()), tex_unit);
        if(checkName(shader_program_id, getVariableName("tex", texture_index, "with_tex_kd") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "with_tex_kd").c_str()), 1);
        if(checkName(shader_program_id, getVariableName("tex", texture_index, "tex_ka") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "tex_ka").c_str()), tex_unit);
        if(checkName(shader_program_id, getVariableName("tex", texture_index, "with_tex_ka") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "with_tex_ka").c_str()), 1);

		glUseProgram(0);
    }


	  /*
    This function checks for the variable names in the shader program shader_program_id
    */
    inline bool checkName(int shader_program_id, std::string variable_name)
    {

        int ret = glGetUniformLocation(shader_program_id, variable_name.c_str());
        if(ret == -1 && with_error_check){
           // std::cout << ret << " [ERROR] - LightSource " << index << "  - Cannot find shader program variable " << variable_name << " (program: "<< shader_program_id << ").\nDid you add the right variable name?" << std::endl; 
            return false;
        }
        return true;
    }




	/*
    Assemble a varibale name string from three components.
    */
    inline std::string getVariableName(std::string struct_name, int index, std::string variable_name)
    {
        std::string name = struct_name;
        name.append("[");
        name.append(std::to_string(index));
        name.append("].");
        name.append(variable_name);
        return name;
    }

}Texture;


/*
Texture material data. 
Stores the texture reflection properties. 

Expects to find a texture struct in the shader code as follows

uniform struct Textures {
	sampler2D tex_kd; // diffuse texture
	bool with_tex_kd; // default is false
	sampler2D tex_ka; // ambient texture
	bool with_tex_ka; // 
	sampler2D tex_ks; // specular texture
	bool with_tex_ks;
	sampler2D tex_bump; // specular texture
	bool with_tex_bump; // bumpmap
	int mode;  // 0: replace, 1: modulate, 2: decal, default = 1

} tex[1];

using int texture_index; // the texture index of this material in the glsl texture lib

*/
typedef struct TexMaterial
{
	// number of texturs
	int					num_textures;
	Texture				diff;  // diffuse textures
	Texture				spec; // specular texturs
	Texture				ambi; // ambient textures
	Texture				bump; // bump map
	Texture				env; // environment map. 

	bool				with_tex_kd; 
	bool				with_tex_ks; 
	bool				with_tex_ka; 
	bool				with_tex_kb; 
	bool				with_tex_ke; 

	// the glsl texture lib index. 
	int					texture_index;

	TextureMode			tex_mode;
	float				texture_multiplier;		

	  // error count, a helper to issue warning.
    int      error_count;
	bool	 with_error_check;


	TexMaterial() {
		num_textures = 0;
		error_count = 0;
		with_error_check = true;
		tex_mode = REPLACE;
		texture_index = -1;
		texture_multiplier = 0.7;
		with_tex_ka = false;
		with_tex_kd = false; 
		with_tex_ks = false;
		with_tex_kb = false;
		with_tex_ke = false;
	}


    /*
    The function passes all the uniform variables to the passed program.
    Note that the shader program must use the correct variable names.
    @param program_id - the shader program id as integer
    */
    inline void apply(int shader_program_id)
    {
		glUseProgram(shader_program_id );

		if(texture_index == -1) {
			// this material has no textures
			if(checkName(shader_program_id, "texture_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_index" ), texture_index);
			if(checkName(shader_program_id, "with_textures" )) glUniform1i(glGetUniformLocation(shader_program_id, "with_textures" ), 0);
			//if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), 0.0);
			return; 
		}

		if(checkName(shader_program_id, "texture_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_index" ), texture_index);
		if(checkName(shader_program_id, "with_textures" )) glUniform1i(glGetUniformLocation(shader_program_id, "with_textures" ), 1);
		//if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), texture_multiplier);

		
		glEnable(GL_TEXTURE_2D);
		if( with_tex_kd){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diff.tex_id);
			if(checkName(shader_program_id, getVariableName("tex", texture_index, "tex_kd") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "tex_kd").c_str()), 0);
			if(checkName(shader_program_id, getVariableName("tex", texture_index, "with_tex_kd") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "with_tex_kd").c_str()), 1);
        }

 		if( with_tex_ka){
			glEnable(GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ambi.tex_id);
			if(checkName(shader_program_id, getVariableName("tex", texture_index, "tex_ka") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "tex_ka").c_str()), 1);
			if(checkName(shader_program_id, getVariableName("tex", texture_index, "with_tex_ka") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "with_tex_ka").c_str()), 1);
		}

      /*  glUseProgram(shader_program_id );
		if (checkName(shader_program_id, "tex_kd")) {
			diff.tex_loc = glGetUniformLocation(shader_program_id, "tex_kd");
			glUniform1i(diff.tex_loc, 0);
		}*/
		glUseProgram(shader_program_id );
     
        glUseProgram(0);
    }

	
    /*
    This function checks for the variable names in the shader program shader_program_id
    */
    inline bool checkName(int shader_program_id, std::string variable_name)
    {

        int ret = glGetUniformLocation(shader_program_id, variable_name.c_str());
        if(ret == -1 && error_count < 1 & with_error_check){
            std::cout << ret << " [ERROR] - Texture - Cannot find shader program variable " << variable_name << " (program: "<< shader_program_id << ").\nDid you add the right variable name?" << std::endl; 
			error_count++;
			return false;
        }
        return true;
    }



	
	/*
    Assemble a varibale name string from three components.
    */
    inline std::string getVariableName(std::string struct_name, int index, std::string variable_name)
    {
        std::string name = struct_name;
        name.append("[");
        name.append(std::to_string(index));
        name.append("].");
        name.append(variable_name);
        return name;
    }


}TexMaterial;


};