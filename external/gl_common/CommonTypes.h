/*
This code is part of CS/CPRE/ME 557 - Computer Graphics

Datatype declaration for light and material.
The file declares two datatypes: Material and LightSources, 
which can be used to store material and light parameters, obviously. 

Types included:
- Material: 3D object material
- Properties: common 3D object propertie
- LightSource: representation of a light source. 


Rafael Radkowski
Iowa State University
November 2018
rafael@iastate.edu

All copyrights reserved. 

//---------------------------------------------------------------------
Last edited:

Feb 20, 2020, RR
	- Added a texture struct. 
*/


#ifndef COMMONTYPES
#define COMMONTYPES


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


/*
This struct is a helper which contains material data. 
Note that this struct expects to find a struct datatype in the glsl
shader program for the material

Use

// The material parameters
uniform struct Material {
    vec3  diffColor;
 	float diffInt;
 	vec3  ambColor;
 	float ambInt;
 	vec3  specColor;
 	float specInt;
 	float shininess;
} mat[material_index];
			
The code will not work otherwise (or you have to adapt the names).

Note that the example shader code creates just one mat[1] object per program. 

*/
typedef struct Material
{

	// material name
	std::string	name;

    // textrue id
	std::string map_diffuse;
	std::string map_ambient;
	std::string map_specular;
	int			texture_id; // the texture lib id

    // model matrix
	glm::mat4 model_matrix;

    // Material
    glm::vec3 diffuse_mat;
    float     diffuse_int;
    glm::vec3 ambient_mat;
    float     ambient_int;
    glm::vec3 specular_mat;
    float     specular_int;
    float     specular_s;

    // error count, a helper to issue warning.
    int      error_count;
	bool	 with_error_check;

	// material index
	// this is the glsl material index, the location in the 
	// gpu material struct. 
	int		material_index;

	Material(){
		//texture_id = -1;
	
		material_index = 0;
		model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 

        diffuse_mat = glm::vec3(0.8,0.8,0.8);
        diffuse_int = 1.0;
        ambient_mat = glm::vec3(0.8,0.8,0.8);
        ambient_int = 1.0;
        specular_mat = glm::vec3(0.8,0.8,0.8);
        specular_int = 1.0;
        specular_s = 12.0;

		name = "Material";

        error_count = 0;
		with_error_check = true;

		map_diffuse = "";
		map_ambient = "";
		map_specular = "";
		texture_id = -1;
		
	}


    /*
    The function passes all the uniform variables to the passed program.
    Note that the shader program must use the correct variable names.
    @param program_id - the shader program id as integer
    */
    inline void setAllUniform(int program_id )
    {
		
        glUseProgram(program_id );
		if(checkName(program_id, "material_index" )) glUniform1i(glGetUniformLocation(program_id, "material_index" ), material_index);

        if(checkName(program_id, getVariableName("mat", material_index, "diffColor") )) glUniform3fv(glGetUniformLocation(program_id, getVariableName("mat", material_index, "diffColor").c_str() ), 1, &diffuse_mat[0]);
        if(checkName(program_id, getVariableName("mat", material_index, "diffInt") )) glUniform1f(glGetUniformLocation(program_id, getVariableName("mat", material_index, "diffInt").c_str() ), diffuse_int);
        if(checkName(program_id, getVariableName("mat", material_index, "ambColor") )) glUniform3fv(glGetUniformLocation(program_id,  getVariableName("mat", material_index, "ambColor").c_str() ), 1, &ambient_mat[0]);
        if(checkName(program_id, getVariableName("mat", material_index, "ambInt") )) glUniform1f(glGetUniformLocation(program_id, getVariableName("mat", material_index, "ambInt").c_str() ), ambient_int);
        if(checkName(program_id, getVariableName("mat", material_index, "specColor") )) glUniform3fv(glGetUniformLocation(program_id, getVariableName("mat", material_index, "specColor").c_str()), 1, &specular_mat[0]);
        if(checkName(program_id, getVariableName("mat", material_index, "specInt") )) glUniform1f(glGetUniformLocation(program_id, getVariableName("mat", material_index, "specInt").c_str()), specular_int);
        if(checkName(program_id, getVariableName("mat", material_index, "shininess") )) glUniform1f(glGetUniformLocation(program_id, getVariableName("mat", material_index, "shininess").c_str()), specular_s);

        glUseProgram(0);
    }

	inline void apply(int shader_program_id) {
		setAllUniform( shader_program_id);
	}



    /*
    This function checks for the variable names in the shader program shader_program_id
    @param program_id - the shader program id as integer.
    @param name - the variable name
    @return true - if the name exists, false otherwise. 
    */
    inline bool checkName(int program_id, std::string name)
    {
        
        int ret = glGetUniformLocation(program_id, name.c_str());
        if(ret == -1 && error_count < 8 && with_error_check){
            std::cout << ret << " [ERROR] - Material - Cannot find shader program variable " << name << ".\nDid you add the right variable name?" << std::endl; 
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


} Material;


/*
This struct is a helper which contains the material properties for one object, including 
the texture coordinates and the materials. 
. 
Note that this struct expects to find a struct datatype in the glsl
shader program for the properties and the materials. 

Use
// The material parameters
uniform struct Material {
    vec3  diffColor;
 	float diffInt;
 	vec3  ambColor;
 	float ambInt;
 	vec3  specColor;
 	float specInt;
 	float shininess;
} mat[1];
for materials. 
and 

uniform sampler2D  tex[MAX_TEX];
for textures;
	
The code will not work otherwise (or you have to adapt the names).

Note that the example shader code creates just one mat[1] object per program. 

*/
typedef struct Properties
{
    // The object's material
    Material    mat;

     // model matrix
	glm::mat4 model_matrix;

    // id's of assigned textures
    std::vector<unsigned int> texture_id;

    // error count, a helper to issue warning.
    int      error_count;
	bool	 with_error_check;

    Properties()
    {
        model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 
        error_count = 0;
		with_error_check = true;
    }


    /*
    The function passes all the uniform variables to the passed program.
    Note that the shader program must use the correct variable names.
    @param program_id - the shader program id as integer
    */
    inline void setAllUniform(int program_id )
    {
        mat.setAllUniform(program_id);

       // glUseProgram(program_id );
       // glUseProgram(0);
    }


    /*
    This function checks for the variable names in the shader program shader_program_id
    @param program_id - the shader program id as integer.
    @param name - the variable name
    @return true - if the name exists, false otherwise. 
    */
    inline bool checkName(int program_id, std::string name)
    {
        
        int ret = glGetUniformLocation(program_id, name.c_str());
        if(ret == -1 && error_count < 2 + texture_id.size() && with_error_check){
            std::cout << ret << " [ERROR] - Properties - Cannot find shader program variable " << name << ".\nDid you add the right variable name?" << std::endl; 
            error_count++;
            return false;
        }
        return true;
    }



}Properties;



/*
Enumeration to set the light type.
This information is passed to a shader program
and can be used to adapt the light and reflection model.
*/
typedef enum
{
    POINT,
    SPOT,
    DIRECT
} LightType;


/*
This struct is a helper which contains variables to store light soruce data.
Note that this struct expects to find a struct datatype in the glsl
shader program.
Use 

// The material parameters
uniform struct LightSource {
    vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float cutoff_in; // inner
	float cutoff_out; // outer
	float k1;    //attenuation
	float k2;    //attenuation
	bool used;
	int  type;  //0:point, 1:spot, 2:directional
} light[MAX_LIGHTS];

*/
typedef struct _LightSource
{

    // int light index
    // Set this values to a number 0-MAX_LIGHT
    // MAX_LIGHT is currently 10 in the example
    unsigned int    index;

    // position and direction
    glm::vec3 pos;
    glm::vec3 dir;

    // color of the light
    glm::vec3  color;
	
    // intensity of the light
    float intensity;

    // The example shader code creates 
    // 10 lights (MAX_LIGHTS) and memory for it.
    // The lights are only processed if the variable used==true.
    // Since the uniform variable is false by default, unused lights 
    // will not be processed. 
    bool       used;

    // spotlight
    float     cutoff_in; // inner cutoff
    float     cutoff_out; // outer cutoff

    // attenuation
    float     k1; // linear
    float     k2; // quadratic

    // type of the light
    LightType   type;

     // error count, a helper to issue warning.
    int      error_count;
	bool	with_error_check;

	_LightSource(){
        index = -1;
        pos =  glm::vec3(0.0,0.0,0.0);
        dir =  glm::vec3(0.0,0.0,0.0);
        color =  glm::vec3(1.0,1.0,1.0);
        cutoff_in = 0.0;
        cutoff_out = 0.0;
        intensity = 1.0;
        k1 = 0.0;
        k2 = 0.0;
        type = POINT;
        used = true;

        error_count = 0;
		with_error_check = true;
	}


    /*
    The function passes all the uniform variables to the passed program.
    Note that the shader program must use the correct variable names.
    @param program_id - the shader program id as integer
    */
    inline void setAllUniform(int shader_program_id)
    {
        glUseProgram(shader_program_id );
        if(checkName(shader_program_id, getVariableName("light", index, "position") )) glUniform3fv(glGetUniformLocation(shader_program_id , getVariableName("light", index, "position").c_str()), 1, &pos[0]);
        if(checkName(shader_program_id, getVariableName("light", index, "direction") )) glUniform3fv(glGetUniformLocation(shader_program_id , getVariableName("light", index, "direction").c_str()), 1, &dir[0]);
        if(checkName(shader_program_id, getVariableName("light", index, "color") )) glUniform3fv(glGetUniformLocation(shader_program_id , getVariableName("light", index, "color").c_str()), 1, &color[0]);
        if(checkName(shader_program_id, getVariableName("light", index, "intensity") )) glUniform1f(glGetUniformLocation(shader_program_id , getVariableName("light", index, "intensity").c_str()), intensity);
        if(checkName(shader_program_id, getVariableName("light", index, "used") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("light", index, "used").c_str()), (int)used);
        if(checkName(shader_program_id, getVariableName("light", index, "type") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("light", index, "type").c_str()), (int)type);
        if(checkName(shader_program_id, getVariableName("light", index, "cutoff_in") )) glUniform1f(glGetUniformLocation(shader_program_id , getVariableName("light", index, "cutoff_in").c_str()), cutoff_in);
        if(checkName(shader_program_id, getVariableName("light", index, "cutoff_out") )) glUniform1f(glGetUniformLocation(shader_program_id , getVariableName("light", index, "cutoff_out").c_str()), cutoff_out);
        if(checkName(shader_program_id, getVariableName("light", index, "k1") )) glUniform1f(glGetUniformLocation(shader_program_id , getVariableName("light", index, "k1").c_str()), k1);
        if(checkName(shader_program_id, getVariableName("light", index, "k2") )) glUniform1f(glGetUniformLocation(shader_program_id , getVariableName("light", index, "k2").c_str()), k2);
        glUseProgram(0);
    }

	  
	inline void apply(int shader_program_id) {
		setAllUniform( shader_program_id);
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

}LightSource;









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

		if(checkName(shader_program_id, "texture_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_index" ), texture_index);
		
		// Fetch the texture location and set the parameter to 0.
		// Note that 0 is the number of the texture unit GL_TEXTURE0.
		int location = glGetUniformLocation(shader_program_id, getVariableName("tex", texture_index, "tex_kd").c_str());
		glUniform1i(location, tex_unit);

		// set the texture to active. 
		int u = glGetUniformLocation(shader_program_id, getVariableName("tex", texture_index, "with_tex_kd").c_str());
		glUniform1i(u, 1);

		if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), texture_multiplier);

		glUseProgram(shader_program_id);
	}
	

	inline void apply(int shader_program_id)
    {
		glUseProgram(shader_program_id );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_id);

		if(checkName(shader_program_id, "texture_index" )) glUniform1i(glGetUniformLocation(shader_program_id, "texture_index" ), texture_index);
		if(checkName(shader_program_id, "texture_multiplier" )) glUniform1f(glGetUniformLocation(shader_program_id, "texture_multiplier" ), texture_multiplier);
		
        if(checkName(shader_program_id, getVariableName("tex", texture_index, "tex_kd") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "tex_kd").c_str()), tex_unit);
        if(checkName(shader_program_id, getVariableName("tex", texture_index, "with_tex_kd") )) glUniform1i(glGetUniformLocation(shader_program_id , getVariableName("tex", texture_index, "with_tex_kd").c_str()), 1);
        
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



*/
typedef struct _TexMaterial
{
	// number of texturs
	int					num_textures;
	Texture				diff;  // diffuse textures
	Texture				spec; // specular texturs
	Texture				ambi; // ambient textures
	Texture				bump; // bump map
	Texture				env; // environment map. 

	TextureMode			tex_mode;
			

	  // error count, a helper to issue warning.
    int      error_count;
	bool	 with_error_check;


	_TexMaterial() {
		num_textures = 0;
		error_count = 0;
		with_error_check = true;
		tex_mode = REPLACE;
	}



	
    /*
    The function passes all the uniform variables to the passed program.
    Note that the shader program must use the correct variable names.
    @param program_id - the shader program id as integer
    */
    inline void apply(int shader_program_id)
    {
        glUseProgram(shader_program_id );
		if (checkName(shader_program_id, "tex_kd")) {
			diff.tex_loc = glGetUniformLocation(shader_program_id, "tex_kd");
			glUniform1i(diff.tex_loc, 0);
		}
	
     
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




}TexMaterial;





}//namespace cs557

#endif