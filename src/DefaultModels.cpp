#include "DefaultModels.h"


DefaultModels::DefaultModels()
{
	draw_plane = false;
}

/*
Create a plane
*/
void DefaultModels::createPlane(float sizeX, float sizeY, unsigned int program)
{
	draw_plane = true;

	//------------------------------------------------
    // Create a plane

    plane0_mat.diffuse_mat = glm::vec3(1.0, 0.4, 0.0);
    plane0_mat.diffuse_int = 1.0;
    plane0_mat.ambient_mat = glm::vec3(1.0, 0.4, 0.0);
    plane0_mat.ambient_int = 0.2;
    plane0_mat.specular_mat = glm::vec3(1.0, 1.0, 1.0);
    plane0_mat.specular_int = 0.6;
    plane0_mat.specular_s = 6.0;


	//string path_fs;
	//string path_vs;
	//FileUtils::Search("../src/fr_lit_scene_ext.vs", path_vs );
	//FileUtils::Search("../src/fr_lit_scene_ext.fs", path_fs );

    // Load the shader program
    //int program = cs557::LoadAndCreateShaderProgram( path_vs,  path_fs);

    // Load the shader program
   // int program = cs557::LoadAndCreateShaderProgram("../fr_lit_scene_ext.vs", "../fr_lit_scene_ext.fs");

    // create the geometry
    plane0.create(sizeX, sizeY, program);
	plane0_mat.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 

    // Add light and material to the program. 
    // The function passes all uniform varibles to the program
   // plane0_mat.apply(plane0.getProgram());

   

     // Fetch the texture location and set the parameter to 0.
    // Note that 0 is the number of the texture unit GL_TEXTURE0.
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, color_texture_idx);
   // int texture_location = glGetUniformLocation(plane0.getProgram(), "tex");
   // glUniform1i(texture_location, 0);


	glUseProgram(0);
}



bool DefaultModels::loadModel(std::string path_and_file, int shader_program, glm::vec3 translation)
{

	// create an instance
    cs557::OBJModel mod;
    _scene_models.push_back(mod);

	int i = _scene_models.size()-1;

	string final_path;
	bool ret = cs557::FileUtils::Search(path_and_file, final_path);

	if (!ret) {
		return false;
	}

    // load the model
    _scene_models[i].create(final_path, shader_program );
    
	// set the transformation
	_scene_models[i].setModelMatrix( glm::translate(glm::mat4(1.0f), translation) ); 

	return true;
 }




bool DefaultModels::createModelGrid(std::string path_and_file, int grid_rows, int grid_columns, int grid_depth, float rows_spacing, float col_spacing, float depth_spacing,
									int shader_program, glm::vec3 start_point  )
{
	// create an instance
    cs557::OBJModel mod;
    _grid_models.push_back(mod);
	_grid_params.push_back(GridParams());

	int i = _grid_models.size()-1;

	string final_path;
	bool ret = cs557::FileUtils::Search(path_and_file, final_path);

	if (!ret) {
		return false;
	}

    // load the model
    _grid_models[i].create(final_path, shader_program );
    
	//----------------------------------------------------------------------
	// calculate the grid parameters


	_grid_params[i].rows = grid_rows;
	_grid_params[i].cols = grid_columns;
	_grid_params[i].depth = grid_depth;
	_grid_params[i].space_cols = rows_spacing;
	_grid_params[i].space_cols = col_spacing;
	_grid_params[i].space_depth = depth_spacing;
	_grid_params[i].start = start_point;

	for(int k = 0; k < grid_depth; k++){
		for (int l = 0; l < grid_rows; l++) {
			for (int j = 0; j < grid_columns; j++) {
				glm::vec3 pos = start_point;
				pos.x = pos.x + j * col_spacing;
				pos.y = pos.y + k * depth_spacing;
				pos.z = pos.z + l * rows_spacing;
				_grid_params[i].position.push_back(pos);
			}
		}
	}

}
 




void DefaultModels::draw(glm::mat4 pm, glm::mat4 vm)
{

	if(draw_plane){
		glUseProgram(plane0.getProgram());
		glBindTexture(GL_TEXTURE_2D, 0 );
		int mu = glGetUniformLocation(plane0.getProgram(), "texture_multiplier");
		glUniform1f(mu, 0.0);
		plane0_mat.apply(plane0.getProgram());
		plane0.draw(pm,vm, plane0_mat.model_matrix);
	}


	// render all mdoels
	for (auto m : _scene_models) {
		m.draw(pm, vm);
	}

	// render the grid
	for (int i = 0; i< _grid_models.size() ; i++) {
		for (auto t : _grid_params[i].position) {
			glm::mat4 mm = glm::translate(glm::mat4(1.0), t);
			_grid_models[i].setModelMatrix(glm::translate(glm::mat4(1.0), t));
			_grid_models[i].draw(pm, vm);
		}
	}
}