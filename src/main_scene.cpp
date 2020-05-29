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
#include "Window.h" // the windows
#include "OpenGLDefaults.h" // some open gl and glew defaults
#include "VertexBuffers.h"  // create vertex buffer object
#include "ShaderProgram.h"  // create a shader program
#include "CommonTypes.h"  // types that all projects use
#include "RenderToTexture.h" // for the prerender texture

// Geometry
#include "ModelCoordinateSystem.h"
#include "ModelPlane.h"
#include "ModelObj.h"
#include "ModelOBJExt.h"
#include "ModelSphere.h"

// the foveated renderer scene
#include "FoveatedRenderingScene.h"


//  For eye tracking data
#include "EyeDataReader.h"
#include "EyeData.h"
#include "EyeRenderHelpers.h"

#include "ArgParser.h"
#include "TimeUtils.h"
#include "TimeMeasurement.h"

using namespace std;
using namespace cs557;


namespace ns_main_scene{
//------------------------------------------------------------
//
//	Some global variables to keep track of

// The handle to the window object
GLFWwindow *window = NULL;

// Transformation pipeline variables
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix;       // Store the view matrix


//------------------------------------------------------------
//
// EYE TRACKING DATA
std::string gaze_data_file = "../data/EyeCoordinates.txt"; 

//---------------------------------------------------------------
// Foveated rendering parameters
typedef struct FRParams
{

	int window_height;
	int window_width;
	int OSZoom;
	int tex_size;
	float eye_radius;
	bool with_fr;
	int max_iterations; // -1 for infinite
	int row_spheres;
	int num_lights;
	string output_file; 


	FRParams(){
		window_height = 1024;//1200;
		window_width = 1024;//1600;
		OSZoom = 1.0;
		tex_size = 128;
		eye_radius = 0.1;
		with_fr  = false;
		max_iterations = -1;
		row_spheres = 3;
		num_lights = 3;
		output_file = "data.csv";
	}

}FRParams;


FRParams frparams;

FoveatedRenderingScene* frrenderer;



string object_str[] = { "../models/teapot.obj",
						"../models/teapot_t.obj"};
glm::vec3 object_t[] = {glm::vec3(2.0, 0.0, 0.0),
						glm::vec3(-2.0, 0.0, 0.0)};

std::vector<std::string> object_to_load(object_str, object_str + sizeof(object_str) / sizeof(std::string));
std::vector<glm::vec3> object_transformation(object_t, object_t + sizeof(object_t) / sizeof(glm::vec3));


string object_park_str[] = { "../models/ParkObj2/park.obj"};
glm::vec3 object_park_t[] = {glm::vec3(0.0, 0.0, 0.0)};


std::vector<std::string> object_park(object_park_str, object_park_str + sizeof(object_park_str) / sizeof(std::string));
std::vector<glm::vec3> object_park_transformation(object_park_t, object_park_t + sizeof(object_park_t) / sizeof(glm::vec3));




//-------------------------------------------------------------------------------------------------
// Time measurement


string time_points[] = { "start_frame",
						 "start_fbo",
						 "end_fbo",
						 "start_hr",
						 "end_hr",
						 "start_swap",
						 "end_swap"};

std::vector<std::string> time_points_str(time_points, time_points + sizeof(time_points) / sizeof(std::string));
int	time_points_str_size = time_points_str.size();

//-----------------------------------------------------------------------------------------
// This is the callback we'll be registering with GLFW for keyboard handling.
// The only thing we're doing here is setting up the window to close when we press ESC
void my_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //cout << key << endl << flush;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if(key == 87 && action == GLFW_PRESS) // w
    {

    }
    else if(key == 83 && action == GLFW_PRESS)//s
    {

    }
    else if(key == 82 && action == GLFW_PRESS)  // r
    {

    }
    else if(key == 70 && action == GLFW_PRESS) // f
    {

    }
    else if(key == 65 && action == GLFW_PRESS) // a
    {

    }
   
}


};

using namespace ns_main_scene;






//------------------------------------------------------------
//
//	INIT THE APPLICATION
void InitScene(void)
{
    // Projection transformations
	projectionMatrix = glm::perspective(1.57f, (float)frparams.window_height / (float)frparams.window_width, 0.1f, 100.f);
	viewMatrix = glm::lookAt(glm::vec3(1.0f, -4.0f, 3.f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Init the GLFW Window
    window = cs557::initWindow(frparams.window_height, frparams.window_width);
	cs557::SetCameraSpeed(0.1, 0.1);
   
    // set a keyboard callback
    glfwSetKeyCallback(window, my_key_callback);

    // Initialize the GLEW apis
    cs557::initGlew();

    // Create the scene
    frrenderer  = new  FoveatedRenderingScene(frparams.window_height, frparams.window_width, frparams.tex_size, frparams.OSZoom);
	frrenderer->loadEyeTrackingData(gaze_data_file);
	frrenderer->setEyeRadius(frparams.eye_radius);
	frrenderer->setObjectsToLoad(object_to_load, object_transformation);
	frrenderer->createContent(frparams.num_lights, true);
	frrenderer->enableFR(frparams.with_fr);

	if(frparams.row_spheres == 1)
		frrenderer->addModelGrid("../models/Sphere.obj", 6, 12, 2, 1.2, 1.2, 1.2, glm::vec3(-8, 0.0, -1.0));
	else if(frparams.row_spheres == 2)
		frrenderer->addModelGrid("../models/Sphere.obj", 6, 12, 4, 1.2, 1.2, 1.2, glm::vec3(-8, 0.0, -1.0));
	else if(frparams.row_spheres == 3)
		frrenderer->addModelGrid("../models/Sphere.obj", 6, 12, 6, 1.2, 1.2, 1.2, glm::vec3(-8, 0.0, -1.0));


	// Time measurement
	TimeMeasurement::Init(time_points_str_size, time_points_str);
	TimeMeasurement::SetSaveFile(frparams.output_file);
	TimeMeasurement::AddTimeDiff(0,6, "frame_all");
	TimeMeasurement::AddTimeDiff(1,2, "fbo_queue");
	TimeMeasurement::AddTimeDiff(3,4, "hr_queue");
	TimeMeasurement::AddTimeDiff(5,6, "gpu_all");


	/*
	{ "start_frame",
		"start_fbo",
		"end_fbo",
		"start_hr",
		"end_hr",
		"start_swap",
		"end_swap"};

	*/

}


void InitPark(void){
	
	  // Projection transformations
	projectionMatrix = glm::perspective(1.57f, (float)frparams.window_height / (float)frparams.window_width, 0.1f, 2000.f);
	viewMatrix = glm::lookAt(glm::vec3(-200.0f, -200.0f, 90.f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Init the GLFW Window
    window = cs557::initWindow(frparams.window_height, frparams.window_width);
	cs557::SetCameraSpeed(0.3, 0.3);
   
    // set a keyboard callback
    glfwSetKeyCallback(window, my_key_callback);

    // Initialize the GLEW apis
    cs557::initGlew();

	cs557::SetCameraSpeed(10.0, 10.0);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    // Create the scene
    frrenderer  = new  FoveatedRenderingScene(frparams.window_height, frparams.window_width, frparams.tex_size, frparams.OSZoom);
	frrenderer->loadEyeTrackingData(gaze_data_file);
	frrenderer->setEyeRadius(frparams.eye_radius);
	frrenderer->setObjectsToLoad(object_park, object_park_transformation);
	frrenderer->createContent(frparams.num_lights, false);
	frrenderer->enableFR(frparams.with_fr);



	// Time measurement
	TimeMeasurement::Init(time_points_str_size, time_points_str);
	TimeMeasurement::SetSaveFile(frparams.output_file);
	TimeMeasurement::AddTimeDiff(0,6, "frame_all");
	TimeMeasurement::AddTimeDiff(1,2, "fbo_queue");
	TimeMeasurement::AddTimeDiff(3,4, "hr_queue");
	TimeMeasurement::AddTimeDiff(5,6, "gpu_all");

}


void InitTest(void){


	string test_obj_str[] = { "../models/sphere_t.obj",
							  "../models/sphere_t4.obj",
							  "../models/sphere_t2.obj",
							  "../models/sphere_t3.obj",
							  "../models/sphere_t5.obj",
							  "../models/sphere_t6.obj",
							   "../models/sphere_t2.obj",
							    "../models/sphere_t.obj"};

	glm::vec3 test_obj_t[] = { glm::vec3(2.0, 0.0, 0.0),
							   glm::vec3(-2.0, 0.0, 0.0),
							   glm::vec3(2.0, 2.0, 0.0),
							   glm::vec3(-2.0, 2.0, 0.0),
							    glm::vec3(2.0, -2.0, 0.0),
							   glm::vec3(-2.0, -2.0, 0.0),
							     glm::vec3(2.0, -4.0, 0.0),
							   glm::vec3(-2.0, -4.0, 0.0)};

	std::vector<std::string> object_to_load2(test_obj_str, test_obj_str + sizeof(test_obj_str) / sizeof(std::string));
	std::vector<glm::vec3> object_transformation2(test_obj_t, test_obj_t + sizeof(test_obj_t) / sizeof(glm::vec3));
	
	  // Projection transformations
	projectionMatrix = glm::perspective(1.57f, (float)frparams.window_height / (float)frparams.window_width, 0.1f, 400.f);
	viewMatrix = glm::lookAt(glm::vec3(1.0f, -4.0f, 3.f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Init the GLFW Window
    window = cs557::initWindow(frparams.window_height, frparams.window_width);
	cs557::SetCameraSpeed(0.3, 0.3);
   
    // set a keyboard callback
    glfwSetKeyCallback(window, my_key_callback);

    // Initialize the GLEW apis
    cs557::initGlew();

    // Create the scene
    frrenderer  = new  FoveatedRenderingScene(frparams.window_height, frparams.window_width, frparams.tex_size, frparams.OSZoom);
	frrenderer->loadEyeTrackingData(gaze_data_file);
	frrenderer->setEyeRadius(frparams.eye_radius);
	frrenderer->setObjectsToLoad(object_to_load2, object_transformation2);
	frrenderer->createContent(frparams.num_lights, true);
	frrenderer->enableFR(frparams.with_fr);



	// Time measurement
	TimeMeasurement::Init(time_points_str_size, time_points_str);
	TimeMeasurement::SetSaveFile(frparams.output_file);
	TimeMeasurement::AddTimeDiff(0,6, "frame_all");
	TimeMeasurement::AddTimeDiff(1,2, "fbo_queue");
	TimeMeasurement::AddTimeDiff(3,4, "hr_queue");
	TimeMeasurement::AddTimeDiff(5,6, "gpu_all");

}




//------------------------------------------------------------
//
//	RENDER THE SCENE


void DrawScene(void)
{
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 0 );


    // Init the view matrix. 
    cs557::InitControlsViewMatrix(viewMatrix);


	static const GLfloat clear_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	static const GLfloat clear_depth[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST); // ignore this line
    int count = 0;
	int mean_cout = 0;

    while (!glfwWindowShouldClose(window))
    {


		TimeMeasurement::Record(0);

        // Clear the entire buffer with our green color (sets the background to be green).
        glClearBufferfv(GL_COLOR, 0, clear_color);
        glClearBufferfv(GL_DEPTH, 0, clear_depth);

     
		// update the camera values.
		// Note that this line changes the view matrix.
		glm::mat4 rotated_view =  cs557::GetCamera().getViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) ;
       
		auto t1 = std::chrono::high_resolution_clock::now();

		frrenderer->draw(projectionMatrix, rotated_view);


        glUseProgram(0);

       
	   TimeMeasurement::Record(5);

        // Swap the buffers so that what we drew will appear on the screen.
        glfwSwapBuffers(window);
        glfwPollEvents();

		TimeMeasurement::Record(6);

		count++; // increase count before average calculation. 



		TimeMeasurement::CalcMeanTimes();
		TimeMeasurement::Write();
		TimeMeasurement::Display();



		if( frparams.max_iterations > 0 && count == frparams.max_iterations) {
			std::ofstream fout("result", std::ofstream::out );
			fout <<  TimeMeasurement::GetMeanTime(0) << "\n";
			fout <<  TimeMeasurement::GetMeanTime(3);
			fout.close();
			cout << "Write output file" << endl;
		}

       
	    if( frparams.max_iterations > 0 && count == frparams.max_iterations) {
			exit(1);
		}

	
    }
}










//-w 1024 -h 1024 -tex 256 -eye 0.1 -fr -o ./data/test1.csv -end 5000 -num 3

//------------------------------------------------------------
//
//	RENDER THE SCENE
int main(int argc,  char **argv)
{
    cout << "Foveated rendering test app." << endl;
    cout << "Rafael Radkowski\nrafael@iastate.edu\nIowa State University\n" << endl;
  
    cout << "Example arguments: -w 1024 -h 1024 -tex 256 -eye 0.1 -fr -o ./data/test1.csv -end 5000 -num 3 -light 3" << endl;



	arlab::Arguments arg = arlab::ArgParser::Parse(argc, argv);
	arlab::ArgParser::Display();

	frparams.window_height = arg.window_height;
	frparams.window_width = arg.window_width;
	frparams.OSZoom = arg.zoom;
	frparams.tex_size = arg.tex_size;
	frparams.eye_radius = arg.eye_radius;
	frparams.with_fr = arg.WITH_FR;
	frparams.output_file = arg.output_file;
	frparams.max_iterations = arg.end_cout;
	frparams.row_spheres = arg.row_spheres;
	frparams.num_lights = arg.num_lights;

	//Change output file name
	int idx = frparams.output_file.find_last_of(".");
	if(idx > 0){
		frparams.output_file = frparams.output_file.substr(0,idx);
		frparams.output_file.append("_");
		frparams.output_file.append(TimeUtils::GetCurrentDateTime());
		frparams.output_file.append(".csv");
	}

	cout << "Write data to " << frparams.output_file << endl;

	std::ofstream fout(frparams.output_file, std::ofstream::out );
	fout << "FR," << frparams.with_fr <<"\n";
	fout << "window_height," << frparams.window_height <<"\n";
	fout << "window_width," << frparams.window_width <<"\n";
	fout << "tex_size," << frparams.tex_size <<"\n";
	fout << "FReye_radius," << frparams.eye_radius << "\n";
	fout << "row_spheres," << frparams.row_spheres << "\n";
	fout << "num_lights," << frparams.num_lights << "\n\n";
	fout << "max_iterations," << frparams.max_iterations << "\n\n";
	fout.close();


	



	// Init the scene 
    InitScene();
	//InitPark();
	//InitTest();

	// start drawing
	DrawScene();



    return 1;
}
