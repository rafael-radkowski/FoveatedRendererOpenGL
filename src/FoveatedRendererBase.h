#pragma once
/*












*/


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
#include "ShaderProgram.h"  // create a shader program
#include "OpenGLDefaults.h"
#include "Window.h"
#include "CommonTypes.h"  // types that all projects use
#include "RenderToTexture.h" // for the prerender texture
#include "ModelCoordinateSystem.h"

//  For eye tracking data
#include "EyeDataReader.h"
#include "EyeData.h"
#include "EyeRenderHelpers.h"

#include "TimeUtils.h"
#include "FileUtils.h"
#include "TimeMeasurement.h"




class FoveatedRendererBase {

public:
	
	/*
	Init the renderer.
	@param window_height - the height of the output window / full resolution renderer in pixel. 
	@param window_width - the width of the output window / full resolution renderer in pixel. 
	@param fbo_size - the height and width of the frame buffer object render buffer in pixel. 
		This is the size of the hidden szene.
	@param oszoom -  a zoom factor to account for the Windows 10 display zoom. 
	*/
	FoveatedRendererBase(int window_height, int window_width, int fbo_size, int oszoom = 1 );
	~FoveatedRendererBase();


	/*
	Load eye tracking data.
	@param - path_and_file - string containig a file to eye tracking data. 
	*/
	bool loadEyeTrackingData(std::string path_and_file);


	/*
	Set the eye radius for the full resolution renderer. 
	The value must be a parameter between 0.01 and 1
	@param radius - as fraction of the clip space coordinate frame. 
	*/
	bool setEyeRadius(float radius);


	/*
	Enable or disable FR gaze function
	@param enable - true enables the gaze helper. 
	*/
	bool enableGaze(bool enable);



	/*
	Enable or disable foveated rendering
	*/
	bool enableFR(bool enable);



	/*
	Draw the scene
	@param pm - 4x4 projection matrix
	@param vm - 4x4 view matrix. 
	*/
	bool draw(glm::mat4 pm, glm::mat4 vm);    


protected:

	/*
	Return the foveated rendering program id
	*/
	int getFRProgram(void);


	virtual void renderContent(glm::mat4 pm, glm::mat4 vm) = 0;


private:


	
	/*
	Initialize the fbo content
	*/
	void createPrerendererScene(void);




	int _window_height;
	int _window_width;
	int _fbo_size;
	int _OSzoom; // windows operating system zoom

	float		_eye_radius;

	// gaze helper
	EyeRenderHelpers		_gaze_helper;
	EyeData					_gaze_data;
	std::string				_gaze_data_file;
	bool					_with_gaze_data;


	// fbo
	unsigned int			_fboHidden;
	GLuint					_color_texture_idx;
	GLuint					_depth_texture_idx;

	int						_fr_program;
	bool					_with_fr;
	int						_with_fr_shader_value; // integer to pass to the shader program


	// coordinate syste
	// a corodinate system
	cs557::CoordinateSystem _coordinateSystem;
	glm::mat4				_modelMatrixCoord; 


};