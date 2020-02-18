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
bool FoveatedRenderingScene::createContent(void)
{
	// create a default plane
 	_defaultModels.createPlane(10.0, 10.0, getFRProgram());

	// load all objects from files
	size_t size = _loading_list.size();
	for (int i=0; i<size; i++) {
		_defaultModels.loadModel(_loading_list[i], getFRProgram(), _loading_position[i]);
	}

	// create light sources
	createLights(3);

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
    _lights[0].pos =  glm::vec3(0.0f, -3.0f, 3.0f);
    _lights[0].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
    _lights[0].k1 = 0.1;


    //---------------------------------------------------------
    // Create a second light. 
    _lights[1].pos =  glm::vec3(0.0f, 3.0f, 3.0f);
    _lights[1].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
    _lights[1].color = glm::vec3(0.0f, 0.0f, 1.0f);


    //---------------------------------------------------------
    // Create a third light.
    // It is a spotlight in this case. 
    _lights[2].pos =  glm::vec3(3.0f, 0.0f, 4.0f);
    _lights[2].dir =  glm::vec3(0.0f, 0.0f, 0.0f);
    _lights[2].color = glm::vec3(1.0f, 1.0f, 1.0f);


	// inner and outer cutoff angle must be equal to realize a sharp edge.
	// cutoff_in <= cutoff_out, you invert the effect otherwise. 
	_lights[2].cutoff_in = 0.1;
	_lights[2].cutoff_out = 0.1;
	_lights[2].intensity = 1.0;
	_lights[2].type = cs557::SPOT;



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