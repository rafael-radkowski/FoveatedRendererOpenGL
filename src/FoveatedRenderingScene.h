#pragma once


#include <iostream>
#include <string>
#include <vector>


// local
#include "FoveatedRendererBase.h"
#include "DefaultModels.h" // standard models to render
#include "CommonTypes.h"

class FoveatedRenderingScene : public FoveatedRendererBase
{
public:

	/*
	Init the renderer.
	@param window_height - the height of the output window / full resolution renderer in pixel. 
	@param window_width - the width of the output window / full resolution renderer in pixel. 
	@param fbo_size - the height and width of the frame buffer object render buffer in pixel. 
		This is the size of the hidden szene.
	@param oszoom -  a zoom factor to account for the Windows 10 display zoom. 
	*/
	FoveatedRenderingScene(int window_height, int window_width, int fbo_size, int oszoom = 1 ) ;
		
	~FoveatedRenderingScene();

	/*
	*/
	bool createContent(int num_lights, bool default_plane = true);


	bool setObjectsToLoad(std::vector<std::string>& loading_list, std::vector<glm::vec3>& position);


	bool addModelGrid(std::string path_and_file, int grid_height, int grid_width, int grid_depth,
						float space_height, float space_width, float space_depth, glm::vec3 start_translation);

	 /*
	Create light sources
	*/
	void createLights(int num_lights);


protected:

	
	void renderContent(glm::mat4 pm, glm::mat4 vm);



	//-----------------------------------------------------------------------------

	DefaultModels							_defaultModels;

	std::vector<std::string>				_loading_list;
	std::vector<glm::vec3>					_loading_position;

	// The light source
	std::vector<cs557::LightSource>			_lights;

};