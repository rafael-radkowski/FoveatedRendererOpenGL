#include "FoveatedRenderingScene.h"



FoveatedRenderingScene::FoveatedRenderingScene(int window_height, int window_width, int fbo_size, int oszoom  ): 
			FoveatedRendererBase(window_height, window_width,  fbo_size, oszoom)
{
	

}

FoveatedRenderingScene::~FoveatedRenderingScene()
{


}


bool FoveatedRenderingScene::setObjectsToLoad(std::vector<std::string>& loading_list, std::vector<glm::vec3>& position)
{
	_loading_list = loading_list;
	_loading_position = position;

	return true;
}


bool FoveatedRenderingScene::addModelGrid(std::string path_and_file, int grid_height, int grid_width, int grid_depth,
						float space_height, float space_width, float space_depth, glm::vec3 start_translation)
{
	return _defaultModels.createModelGrid(path_and_file, grid_height, grid_width, grid_depth, space_height, space_width, space_depth,
									getFRProgram(), start_translation);

		
}


/*
*/
bool FoveatedRenderingScene::createContent(int num_lights, bool default_plane)
{
	// create a default plane
	if(default_plane)
 		_defaultModels.createPlane(10.0, 10.0, getFRProgram());

	// load all objects from files
	size_t size = _loading_list.size();
	for (int i=0; i<size; i++) {
		_defaultModels.loadModel(_loading_list[i], getFRProgram(), _loading_position[i]);
	}

	// create light sources
	createLights(num_lights);

	return true;
}



 /*
Create light sources
Light sources on the host system are just parameters that get passed
to the shader programs to fed the light and reflection models. 
You can find the light source and material type declarations 
in CommonTypes.h.
Both types come with a method: setAllUniform(int program),
which passes all type variables as uniform variables to the program. 
*/
void FoveatedRenderingScene::createLights(int num_lights)
{
   

	// The light source

	for( int i=0 ; i< num_lights; i++){
		_lights.push_back( cs557::LightSource());
		_lights[i].index = i;
	}

	//---------------------------------------------------------
    // Create a first light.
	if (_lights.size() >= 1) {
		_lights[0].pos = glm::vec3(0.0f, 100.0f, 50.0f);
		_lights[0].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[0].k1 = 0.2;
		_lights[0].intensity = 2.0;
		_lights[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	if (_lights.size() >= 2) {
		//---------------------------------------------------------
		// Create a second light. 
		// _lights[1].pos =  glm::vec3(0.0f, 3.0f, 3.0f);
		// _lights[1].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
		//  _lights[1].color = glm::vec3(1.0f, 1.0f, 1.0f);

		_lights[1].pos = glm::vec3(0.0f, -100.0f, 500.0f);
		_lights[1].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[1].color = glm::vec3(1.0f, 1.0f, 1.0f);
		_lights[1].intensity = 2.0;
		_lights[1].k1 = 0.2;
	}

	if (_lights.size() >= 3) {
		//---------------------------------------------------------
		// Create a third light.
		// It is a spotlight in this case. 
		//_lights[2].pos =  glm::vec3(3.0f, 0.0f, 4.0f);
		// _lights[2].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
		//_lights[2].color = glm::vec3(1.0f, 1.0f, 1.0f);
		_lights[2].pos = glm::vec3(30.0f, 0.0f, 40.0f);
		_lights[2].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[2].color = glm::vec3(1.0f, 1.0f, 1.0f);


		// inner and outer cutoff angle must be equal to realize a sharp edge.
		// cutoff_in <= cutoff_out, you invert the effect otherwise. 
		_lights[2].cutoff_in = 0.1;
		_lights[2].cutoff_out = 0.1;
		_lights[2].intensity = 1.0;
		_lights[2].type = cs557::SPOT;
	}

	if (_lights.size() >= 4) {
		//---------------------------------------------------------
		// Create a fourth light.
		_lights[3].pos = glm::vec3(0.0f, 10.0f, 4.0f);
		_lights[3].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[3].k1 = 0.2;
		_lights[3].intensity = 2.0;
		_lights[3].color = glm::vec3(0.7f, 0.3f, 0.7f);
	}

	if (_lights.size() >= 5) {
		//---------------------------------------------------------
		// Create a fifth light. 
		// _lights[1].pos =  glm::vec3(0.0f, 3.0f, 3.0f);
		// _lights[1].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
		//  _lights[1].color = glm::vec3(1.0f, 1.0f, 1.0f);

		_lights[4].pos = glm::vec3(0.0f, -10.0f, 3.0f);
		_lights[4].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[4].color = glm::vec3(0.7f, 0.3f, 0.7f);
		_lights[4].intensity = 2.0;
		_lights[4].k1 = 0.2;
	}

	if (_lights.size() >= 6) {
		//---------------------------------------------------------
		// Create a sixth light.
		// It is a spotlight in this case. 
		//_lights[2].pos =  glm::vec3(3.0f, 0.0f, 4.0f);
		// _lights[2].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
		//_lights[2].color = glm::vec3(1.0f, 1.0f, 1.0f);
		_lights[5].pos = glm::vec3(0.0f, 0.0f, 3.0f);
		_lights[5].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[5].color = glm::vec3(0.7f, 0.3f, 0.7f);


		// inner and outer cutoff angle must be equal to realize a sharp edge.
		// cutoff_in <= cutoff_out, you invert the effect otherwise. 
		_lights[5].cutoff_in = 0.1;
		_lights[5].cutoff_out = 0.1;
		_lights[5].intensity = 1.0;
		_lights[5].type = cs557::SPOT;
	}

	if (_lights.size() >= 7) {
		//---------------------------------------------------------
		// Create a seventh light.
		_lights[6].pos = glm::vec3(0.0f, 10.0f, -4.0f);
		_lights[6].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[6].k1 = 0.2;
		_lights[6].intensity = 2.0;
		_lights[6].color = glm::vec3(0.5f, 0.0f, 0.5f);

	}

	if (_lights.size() >= 8) {
		//---------------------------------------------------------
		// Create a eighth light. 
		// _lights[1].pos =  glm::vec3(0.0f, 3.0f, 3.0f);
		// _lights[1].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
		//  _lights[1].color = glm::vec3(1.0f, 1.0f, 1.0f);

		_lights[7].pos = glm::vec3(0.0f, -10.0f, -3.0f);
		_lights[7].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[7].color = glm::vec3(0.5f, 0.0f, 0.5f);
		_lights[7].intensity = 2.0;
		_lights[7].k1 = 0.2;
	}

	if (_lights.size() >= 9) {
		//---------------------------------------------------------
		// Create a ninth light.
		// It is a spotlight in this case. 
		//_lights[2].pos =  glm::vec3(3.0f, 0.0f, 4.0f);
		// _lights[2].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
		//_lights[2].color = glm::vec3(1.0f, 1.0f, 1.0f);
		_lights[8].pos = glm::vec3(0.0f, 0.0f, -3.0f);
		_lights[8].dir = glm::vec3(0.0f, 0.0f, 0.0f);
		_lights[8].color = glm::vec3(0.5f, 0.0f, 0.5f);


		// inner and outer cutoff angle must be equal to realize a sharp edge.
		// cutoff_in <= cutoff_out, you invert the effect otherwise. 
		_lights[8].cutoff_in = 0.1;
		_lights[8].cutoff_out = 0.1;
		_lights[8].intensity = 1.0;
		_lights[8].type = cs557::SPOT;
	}

	//-----------------------------------------------------------
	// Apply the light to the shader program
	glUseProgram(getFRProgram());
	for( int i=0 ; i< num_lights; i++){
		_lights[i].apply(getFRProgram());
	}

}




	
void FoveatedRenderingScene::renderContent(glm::mat4 pm, glm::mat4 vm)
{
	
	_defaultModels.draw(pm, vm);


}