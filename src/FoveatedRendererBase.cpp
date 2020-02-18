
#include "FoveatedRendererBase.h"


FoveatedRendererBase::FoveatedRendererBase(int window_height, int window_width, int fbo_size, int oszoom  ):
	 _window_height(window_height),  _window_width(window_width),  _fbo_size(fbo_size), _OSzoom(oszoom)
{
	_gaze_data_file = "";
	_with_gaze_data = true;
	_with_fr = true;
	_with_fr_shader_value = 1;

	_fboHidden = -1;
	_color_texture_idx = -1;
	_depth_texture_idx = -1;
	_eye_radius = 0.1;


	 // disable frame sync
    glfwSwapInterval(0);

	// create the fbo for the hidden scene.
	createPrerendererScene();


	// create a coordinate system
    // That's a tiny helper to keep orientation
    _coordinateSystem.create(2.5);
	_modelMatrixCoord = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 



	string path_fs;
	string path_vs;

	cs557::FileUtils::Search("../src/fr_lit_scene_ext.vs", path_vs );
	cs557::FileUtils::Search("../src/fr_lit_scene_ext.fs", path_fs );

    // Load the shader program
    _fr_program = cs557::LoadAndCreateShaderProgram( path_vs,  path_fs);


	_gaze_helper.setResolution(oszoom*window_width,oszoom*window_height);
    _gaze_helper.setRadius(0.1);

}


FoveatedRendererBase::~FoveatedRendererBase()
{

}


/*
Load eye tracking data.
@param - path_and_file - string containig a file to eye tracking data. 
*/
bool FoveatedRendererBase::loadEyeTrackingData(std::string path_and_file)
{
	cs557::FileUtils::Search(path_and_file, path_and_file);

	EyeDataReader reader;
    reader.read(path_and_file, &_gaze_data);




	return true;
}


/*
Enable or disable FR gaze function
@param enable - true enables the gaze helper. 
*/
bool FoveatedRendererBase::enableGaze(bool enable)
{
	_with_gaze_data = enable;

	return _with_gaze_data;
}


/*
Enable or disable foveated rendering
*/
bool FoveatedRendererBase::enableFR(bool enable)
{
	_with_fr = enable;
	_with_fr_shader_value = int(enable);

	return _with_fr;
}


/*
Set the eye radius for the full resolution renderer. 
The value must be a parameter between 0.01 and 1
@param radius - as fraction of the clip space coordinate frame. 
*/
bool FoveatedRendererBase::setEyeRadius(float radius)
{
	if (radius > 0.01 && radius < 1.1) {
		_eye_radius = radius;
		_gaze_helper.setRadius(_eye_radius);
		_gaze_helper.setParamsToProgram(_fr_program);
		return true;
	}
	return false;
}

/*
Return the foveated rendering program id
*/
int FoveatedRendererBase::getFRProgram(void)
{
	return _fr_program;
}



/*
Draw the scene
@param pm - 4x4 projection matrix
@param vm - 4x4 view matrix. 
*/
bool FoveatedRendererBase::draw(glm::mat4 pm, glm::mat4 vm)
{

	static const GLfloat clear_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	static const GLfloat clear_depth[] = {1.0f, 1.0f, 1.0f, 1.0f};



	 // set the gaze helper values
    float time = glfwGetTime();
    Eigen::Vector2f pos = _gaze_data.forTime(time);
    _gaze_helper.setEyeCoordinates(_fr_program, pos.x(), pos.y());
	//std::cout << pos.x() << " : " << pos.y() <<std:: endl;

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// FR scene


	TimeMeasurement::Record(1); // outside the loop to prevent that the values are empty when not measuring fbo

	if (_with_fr){

		

		// Bind the frame buffer object
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboHidden);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set the viewport. It must match the texture size.
		glViewport(0, 0, _fbo_size, _fbo_size);

    
		// Clear the entire buffer with our green color.
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, clear_depth);
 

		
        glUseProgram(_fr_program);
        glUniform1i(glGetUniformLocation(_fr_program, "with_fr"),0);
		glUseProgram(_fr_program);


		// render all content
		renderContent(pm, vm);


		// Reset to the regular buffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


		glViewport(0,0, (_OSzoom* _window_width), (_OSzoom* _window_height)); // the defaut window is set to this size

		auto t1_end = std::chrono::high_resolution_clock::now();
	
		glBlitFramebuffer(0,0, _fbo_size, _fbo_size,
						0,0, _OSzoom*_window_width, _OSzoom*_window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);



	}
	TimeMeasurement::Record(2);
	//else
	//{
	//	glUseProgram(_fr_program);
	//	glUniform1i(glGetUniformLocation(_fr_program, "with_fr"),1);
	//	glUseProgram(_fr_program);
	//}




	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// Render the visible scene

	TimeMeasurement::Record(3);

	_coordinateSystem.draw(pm, vm, _modelMatrixCoord);


	// deactivate the foveated renderer
	glUseProgram(_fr_program);
	glUniform1i(glGetUniformLocation(_fr_program, "with_fr"),_with_fr_shader_value);
	glUseProgram(_fr_program);

	// render all content
	renderContent(pm, vm);


    glUseProgram(0);

	TimeMeasurement::Record(4);
   
	return true;

}



/*
This function creates the scene with all invisble objects. 
The are rendered into a texture which can be used as 2d map in the visible scene. 
*/
void FoveatedRendererBase::createPrerendererScene(void)
{

	// This function is part of RenderToTexture.h
	// It creates a texture as target for a pre-render step. 
	// The texture is linked to a framebuffer object (fbo), memory to store the image into.
	// This fbo is linked to a texture, so that we can use it in the  scene.
	//CreateRenderToTexture(tex_size, tex_size, fboHidden, color_texture_idx, depth_texture_idx);
    CreateRenderBuffer(_fbo_size, _fbo_size, _fboHidden, _color_texture_idx, _depth_texture_idx);

    

	// Reset to the regular buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


}
