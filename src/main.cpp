/*
This code is part of CS/CPRE/ME 557 - Computer Graphics

This example demonstrates how one can set up multiple light sources with GLSL
per-fragment-light. The scene incorporates a plane and a teapot model 
along with three light sources. 

- The plane is a two-triangle plane to be found in ModelPlane.h/.cpp
- The teapot comes from a file teapot_t.obj, which is loaded in ModelOBJ.h/.cpp
- Shader code for per-fragment-light: lit_scene.vs/.fs

Light and material types are declared in CommonTypes.h.

Note that this example loads and creates one shader program per 3D model, 
although it is 3x the same model (feel free to change this).

Rafael Radkowski
Iowa State University
November 2018
rafael@iastate.edu

All copyrights reserved. 
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

//  For eye tracking data
#include "EyeDataReader.h"
#include "EyeData.h"
#include "EyeRenderHelpers.h"

#include "ArgParser.h"
#include "TimeUtils.h"

using namespace std;
using namespace cs557;
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
//	Some model  to keep track of

// a corodinate system
cs557::CoordinateSystem coordinateSystem;
glm::mat4 modelMatrixCoord; 


// a plane
cs557::Plane plane0;
Material	 plane0mat;

// a teapot
cs557::OBJModel teapot0;
Material	 	teapot0mat;


string object_str[] = {"../../models/sphere.obj", 
                       "../../models/Cylineder.obj",
                       "../../models/teapot.obj" };

std::vector<std::string> object_to_load(object_str, object_str + sizeof(object_str) / sizeof(std::string));

// all other modesl
vector<cs557::OBJModelExt> scene_models;
vector<Material>	 	scene_models_mat;


// The light source
LightSource     light0;
LightSource     light1;
LightSource     light2;

// a light source.
// I render a sphere to indicate the location of the light source.
cs557::Sphere   sphere;
Material        spheremat;




// a helper plane to render the pre-render content
cs557::Plane display;
Material	 display_mat;
glm::mat4    display_p; // Store the projection matrix
glm::mat4    display_vm;       // Store the view matrix

//---------------------------------------------------------------
// 24 spheres
int window_height = 1024;//1200;
int window_width = 1024;//1600;
int OSZoom = 1.0;
int tex_size = 128;
float eye_radius = 0.1;
bool WITH_FR = false;
int max_iterations = -1;
int row_spheres = 3;
string output_file = "data.csv";

#define _WITH_FR
#define _WITH40
#define _WITH80


//------------------------------------------------------------
//
// EYE TRACKING DATA

EyeRenderHelpers    gaze_helper;
EyeData     gaze_data;
std::string gaze_data_file = "../../data/EyeCoordinates.txt";



//---------------------------------------------------------------------
// Parameters for the hidden scene
GLuint program_hidden = -1;
unsigned int fboHidden = -1;
GLuint color_texture_idx = -1;
GLuint depth_texture_idx = -1;


GLuint program_visble = -1;
//------------------------------------------------------------
//
// Set up our green background color
//GLfloat clear_color[] = {0.6f, 0.7f, 1.0f, 1.0f};
GLfloat clear_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat clear_depth[] = {1.0f, 1.0f, 1.0f, 1.0f};



// keyboard helpers
float kx = 0.0;
float ky = -3.0;
float kz = 3.0;
float delta = 0.5;

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
        ky += delta;
        if(ky  > 5.0) ky = 5.0;
        else if(ky < -5.0) ky = -5.0;
    }
    else if(key == 83 && action == GLFW_PRESS)//s
    {
        ky -= delta;
        if(ky  > 5.0) ky = 5.0;
        else if(ky < -5.0) ky = -5.0;
    }
    else if(key == 82 && action == GLFW_PRESS)  // r
    {
        kz += delta;
        if(kz  > 5.0) kz = 5.0;
        else if(kz < 0.0) kz = 0.0;
    }
    else if(key == 70 && action == GLFW_PRESS) // f
    {
        kz -= delta;
        if(kz  > 5.0) kz = 5.0;
        else if(kz < 0.0) kz = 0.0;
    }
    else if(key == 65 && action == GLFW_PRESS) // a
    {
        kx += delta;
        if(kx  > 5.0) kx = 5.0;
        else if(kx < -5.0) kx = -5.0;

    }
    else if(key == 68 && action == GLFW_PRESS) // d
    {
        kx -= delta;
        if(kx  > 5.0) kx = 5.0;
        else if(kx < -5.0) kx = -5.0;

    }
     else if(key == 84 && action == GLFW_PRESS) // t
    {
    

    }

    // Update the position of the light. 
    light0.pos =  glm::vec3(kx, ky, kz);
    light0.setAllUniform(teapot0.getProgram());
    light0.setAllUniform(plane0.getProgram());
    light0.setAllUniform(sphere.getProgram());

    
    for(int i=0; i< object_to_load.size(); i++){
         light0.setAllUniform(scene_models[i].getProgram());
    }
}




void CreateFoveatedRendererBuffer(void)
{

    gaze_helper.loadBufferPattern("../../data/checkerboard_rot.bmp");
    gaze_helper.setResolution(OSZoom*window_width,OSZoom*window_height);
    gaze_helper.setRadius(eye_radius);

}



/*
This function creates the scene with all invisble objects. 
The are rendered into a texture which can be used as 2d map in the visible scene. 
*/
void CreatePrerendererScene(void)
{

	// This function is part of RenderToTexture.h
	// It creates a texture as target for a pre-render step. 
	// The texture is linked to a framebuffer object (fbo), memory to store the image into.
	// This fbo is linked to a texture, so that we can use it in the  scene.
	//CreateRenderToTexture(tex_size, tex_size, fboHidden, color_texture_idx, depth_texture_idx);
    CreateRenderBuffer(tex_size, tex_size, fboHidden, color_texture_idx, depth_texture_idx);

    

	// Reset to the regular buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


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
void CreateLights(void)
{
    //---------------------------------------------------------
    // Create a first light.
    light0.index = 0;
    light0.pos =  glm::vec3(0.0f, -3.0f, 3.0f);
    light0.dir =  glm::vec3(0.0f, 0.0f, 0.0f);
    light0.k1 = 0.1;

    //------------------------------------------------
    // Create a sphere to indicate the light

	string path_fs;
	string path_vs;

	FileUtils::Search("../lit_scene.vs", path_vs );
	FileUtils::Search("../lit_scene.fs", path_fs );

    // Load the shader program
    int program = cs557::LoadAndCreateShaderProgram( path_vs, path_fs);
    sphere.create(0.1, 30, 40, program);

    // This aligns the light position with the sphere position.
    // Note that the light has not graphical representation.
    // Means, the model must be moved to the light locatoin manually.
    spheremat.model_matrix = glm::translate(glm::mat4(1.0f), light0.pos); 
    spheremat.diffuse_mat = glm::vec3(1.0, 1.0, 0.0);
    spheremat.diffuse_int = 1.5;
    spheremat.ambient_mat = glm::vec3(1.0, 1.0, 0.0);
    spheremat.ambient_int = 20.2;
    spheremat.specular_mat = glm::vec3(1.0, 1.0, 1.0);
    spheremat.specular_int = 0.6;
    spheremat.specular_s = 6.0;

    // This function passes all uniform varibles to the program
    spheremat.setAllUniform(sphere.getProgram());
    light0.setAllUniform(sphere.getProgram());

    glUseProgram(0);


    //---------------------------------------------------------
    // Create a second light. 
    light1.index = 1;
    light1.pos =  glm::vec3(0.0f, 3.0f, 3.0f);
    light1.dir =  glm::vec3(0.0f, 0.0f, 0.0f);
    light1.color = glm::vec3(0.0f, 0.0f, 1.0f);


    //---------------------------------------------------------
    // Create a third light.
    // It is a spotlight in this case. 
    light2.index = 2;
    light2.pos =  glm::vec3(3.0f, 0.0f, 4.0f);
    light2.dir =  glm::vec3(0.0f, 0.0f, 0.0f);
    light2.color = glm::vec3(1.0f, 1.0f, 1.0f);

    // inner and outer cutoff angle must be equal to realize a sharp edge.
    // cutoff_in <= cutoff_out, you invert the effect otherwise. 
    light2.cutoff_in = 0.1;
    light2.cutoff_out = 0.1;
    light2.intensity = 1.0;
    light2.type = SPOT;
}




/*
Creates a teapot model
*/
void CreateTeapotModel(void)
{
    // Define some teapot material parameters
    teapot0mat.diffuse_mat = glm::vec3(0.0, 0.4, 0.6);
    teapot0mat.diffuse_int = 1.5;
    teapot0mat.ambient_mat = glm::vec3(0.0, 0.4, 0.6);
    teapot0mat.ambient_int = 0.2;
    teapot0mat.specular_mat = glm::vec3(1.0, 1.0, 1.0);
    teapot0mat.specular_int = 0.6;
    teapot0mat.specular_s = 6.0;

	string path_fs;
	string path_vs;
	FileUtils::Search("../fr_lit_scene_ext.vs", path_vs );
	FileUtils::Search("../fr_lit_scene_ext.fs", path_fs );

	 // Load the shader program
    int program = cs557::LoadAndCreateShaderProgram( path_vs,  path_fs);

    // Load the shader program
   // int program = cs557::LoadAndCreateShaderProgram("../fr_lit_scene_ext.vs",  "../fr_lit_scene_ext.fs");

    // load a teapot from file 
    teapot0.create("../teapot_t.obj", program );
	teapot0mat.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 

    // Add light and material to the program. 
    // The function passes all uniform varibles to the program
    //teapot0mat.setAllUniform(teapot0.getProgram());
    light0.setAllUniform(teapot0.getProgram());
    light1.setAllUniform(teapot0.getProgram());
    light2.setAllUniform(teapot0.getProgram());

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(teapot0.getProgram(), "with_fr"), 1);

        // Fetch the texture location and set the parameter to 0.
    // Note that 0 is the number of the texture unit GL_TEXTURE0.
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, color_texture_idx);
   // int texture_location = glGetUniformLocation(teapot0.getProgram(), "tex");
   // glUniform1i(texture_location, 0);

    // attach the texture to this program
    gaze_helper.setParamsToProgram(program);

	glUseProgram(0);

}

/*
Create the plan
*/
void CreatePlane(void)
{

    //------------------------------------------------
    // Create a plane

    plane0mat.diffuse_mat = glm::vec3(1.0, 0.4, 0.0);
    plane0mat.diffuse_int = 1.0;
    plane0mat.ambient_mat = glm::vec3(1.0, 0.4, 0.0);
    plane0mat.ambient_int = 0.2;
    plane0mat.specular_mat = glm::vec3(1.0, 1.0, 1.0);
    plane0mat.specular_int = 0.6;
    plane0mat.specular_s = 6.0;


	string path_fs;
	string path_vs;
	FileUtils::Search("../fr_lit_scene_ext.vs", path_vs );
	FileUtils::Search("../fr_lit_scene_ext.fs", path_fs );

    // Load the shader program
    int program = cs557::LoadAndCreateShaderProgram( path_vs,  path_fs);

    // Load the shader program
   // int program = cs557::LoadAndCreateShaderProgram("../fr_lit_scene_ext.vs", "../fr_lit_scene_ext.fs");

    // create the geometry
    plane0.create(10.0, 10.0, program);
	plane0mat.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 

    // Add light and material to the program. 
    // The function passes all uniform varibles to the program
    plane0mat.setAllUniform(plane0.getProgram());
    light0.setAllUniform(plane0.getProgram());
    light1.setAllUniform(plane0.getProgram());
    light2.setAllUniform(plane0.getProgram());

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(plane0.getProgram() , "with_fr"), (int) 1);
   

     // Fetch the texture location and set the parameter to 0.
    // Note that 0 is the number of the texture unit GL_TEXTURE0.
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, color_texture_idx);
   // int texture_location = glGetUniformLocation(plane0.getProgram(), "tex");
   // glUniform1i(texture_location, 0);

    // attach the texture to this program
    gaze_helper.setParamsToProgram(program);

	glUseProgram(0);

}



void LoadModels(void)
{

    for(int i=0; i< object_to_load.size(); i++){

		string path_fs;
		string path_vs;

		FileUtils::Search("../fr_lit_scene_ext.vs", path_vs );
		FileUtils::Search("../fr_lit_scene_ext.fs", path_fs );

        // Load the shader program
        int program = cs557::LoadAndCreateShaderProgram( path_vs,  path_fs);


        // create an instance
        cs557::OBJModelExt mod;
        scene_models.push_back(mod);

		string final_path;
		FileUtils::Search(object_to_load[i], final_path);

        // load the model
        scene_models[i].create(final_path, program );
    
        // The function passes all uniform varibles to the program
        light0.setAllUniform(scene_models[i].getProgram());
        light1.setAllUniform(scene_models[i].getProgram());
        light2.setAllUniform(scene_models[i].getProgram());

        // for foveated rendering
        // attach the texture to this program
        gaze_helper.setParamsToProgram(program);

        glUseProgram(program);
        glUniform1i(glGetUniformLocation(scene_models[i].getProgram(), "with_fr"),1);

        // Fetch the texture location and set the parameter to 0.
        // Note that 0 is the number of the texture unit GL_TEXTURE0.
    //    glActiveTexture(GL_TEXTURE0);
      //  glBindTexture(GL_TEXTURE_2D, color_texture_idx);
     //   int texture_location = glGetUniformLocation(scene_models[i].getProgram(), "tex");
    //    glUniform1i(texture_location, 0);



        glUseProgram(0);

    }

    scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)); 
    scene_models[1].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 1.0f)); 
    scene_models[2].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));; 

}


//------------------------------------------------------------
//
// CREATE THE SCENE
void CreateScene(void)
{

    // Projection transformations
	projectionMatrix = glm::perspective(1.57f, (float)window_height / (float)window_width, 0.1f, 100.f);
	viewMatrix = glm::lookAt(glm::vec3(1.0f, -4.0f, 3.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // create the foveated renderer
    CreateFoveatedRendererBuffer();

    // create a coordinate system
    // That's a tiny helper to keep orientation
    coordinateSystem.create(2.5);
	modelMatrixCoord = glm::translate(glm::mat4(1.0f), glm::vec3(.0f, 0.0f, 0.0f)); 


      // start the prerender scene
    CreatePrerendererScene();

    glEnable(GL_TEXTURE);

        // Activate the texture unit and bind the texture. 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_texture_idx);


    // Create light sources
    CreateLights();

	// Create the prerenderer scene  with the object that casts the shadow
	// The content is rendered into a texture
	CreateTeapotModel();

	// Create the scene with all visible objects which receive the shadow. 
    CreatePlane();

    // Load all remaining modles
    LoadModels();
    
  







}





//------------------------------------------------------------
//
//	INIT THE APPLICATION
void Init(void)
{
    

    // Init the GLFW Window
    window = cs557::initWindow(window_height, window_width);

    // disable frame sync
    glfwSwapInterval(0);

    // set a keyboard callback
    glfwSetKeyCallback(window, my_key_callback);

    // Initialize the GLEW apis
    cs557::initGlew();

    // Create the scene
    CreateScene();

}


//------------------------------------------------------------
//
//	Draw function

void DrawScene(bool fr_on){

       // gaze_helper.bindTexture();

        glm::mat4 rotated_view =  cs557::GetCamera().getViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) ;

        float time = glfwGetTime();
        Eigen::Vector2f pos = gaze_data.forTime(time);


		 // draw the plane
        gaze_helper.setEyeCoordinates(plane0.getProgram(), pos.x(), pos.y());
        glUseProgram(plane0.getProgram());
        if(fr_on)
            glUniform1i(glGetUniformLocation(plane0.getProgram(), "with_fr"),1);
        else
            glUniform1i(glGetUniformLocation(plane0.getProgram(), "with_fr"),0);

        plane0.draw(projectionMatrix, rotated_view, plane0mat.model_matrix);

         // draw the teapot
        gaze_helper.setEyeCoordinates(teapot0.getProgram(), pos.x(), pos.y());
        glUseProgram(teapot0.getProgram());
        if(fr_on)
            glUniform1i(glGetUniformLocation(teapot0.getProgram(), "with_fr"),1);
        else
            glUniform1i(glGetUniformLocation(teapot0.getProgram(), "with_fr"),0);

		teapot0.draw(projectionMatrix, rotated_view, teapot0mat.model_matrix);

        // draw the models
        for(int i=0; i<object_to_load.size(); i++)
        {
            //glUseProgram(scene_models[i].getProgram());
            gaze_helper.setEyeCoordinates(scene_models[i].getProgram(), pos.x(), pos.y());
            gaze_helper.bindTexture();
            glUseProgram( scene_models[i].getProgram());
            if(fr_on)
                glUniform1i(glGetUniformLocation( scene_models[i].getProgram(), "with_fr"), 1);
            else
                glUniform1i(glGetUniformLocation(scene_models[i].getProgram(), "with_fr"),0);

            scene_models[i].draw(projectionMatrix, rotated_view);
        }

		if (row_spheres > 0)
		{
			for(int i=0; i<12; i++){

				scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3( -6.0f + 1.0f * i, 2.0, 3.0f)); 
				glUseProgram( scene_models[0].getProgram());
				if(fr_on)
					glUniform1i(glGetUniformLocation( scene_models[0].getProgram(), "with_fr"), 1);
				else
					glUniform1i(glGetUniformLocation(scene_models[0].getProgram(), "with_fr"),0);

				scene_models[0].draw(projectionMatrix, rotated_view);
			}

			for(int i=0; i<12; i++){

				scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3( -6.0f + 1.0f * i, 3.0, 4.0f)); 
				glUseProgram( scene_models[0].getProgram());
				if(fr_on)
					glUniform1i(glGetUniformLocation( scene_models[0].getProgram(), "with_fr"), 1);
				else
					glUniform1i(glGetUniformLocation(scene_models[0].getProgram(), "with_fr"),0);

				scene_models[0].draw(projectionMatrix, rotated_view);
			}
		}
#ifdef _WITH40
		if (row_spheres > 1)
		{
			  for(int i=0; i<12; i++){

				scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3( -6.0f + 1.0f * i, 1.0, 2.0f)); 
				glUseProgram( scene_models[0].getProgram());
				if(fr_on)
					glUniform1i(glGetUniformLocation( scene_models[0].getProgram(), "with_fr"), 1);
				else
					glUniform1i(glGetUniformLocation(scene_models[0].getProgram(), "with_fr"),0);
            
				scene_models[0].draw(projectionMatrix, rotated_view);
			}

			for(int i=0; i<12; i++){

				scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3( -6.0f + 1.0f * i, 0.0, 1.0f)); 
				glUseProgram( scene_models[0].getProgram());
				if(fr_on)
					glUniform1i(glGetUniformLocation( scene_models[0].getProgram(), "with_fr"), 1);
				else
					glUniform1i(glGetUniformLocation(scene_models[0].getProgram(), "with_fr"),0);

				scene_models[0].draw(projectionMatrix, rotated_view);
			}
		}
#endif


#ifdef _WITH80

		if (row_spheres > 2)
		{
			for(int i=0; i<12; i++){

				scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(  -6.0f + 1.0f * i, 4.0, 5.0f)); 
				glUseProgram( scene_models[0].getProgram());
				if(fr_on)
					glUniform1i(glGetUniformLocation( scene_models[0].getProgram(), "with_fr"), 1);
				else
					glUniform1i(glGetUniformLocation(scene_models[0].getProgram(), "with_fr"),0);

				scene_models[0].draw(projectionMatrix, rotated_view);
			}

			for(int i=0; i<12; i++){

				scene_models[0].prop.model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3( -6.0f + 1.0f * i, 5.0, 6.0f)); 
				glUseProgram( scene_models[0].getProgram());
				if(fr_on)
					glUniform1i(glGetUniformLocation( scene_models[0].getProgram(), "with_fr"), 1);
				else
					glUniform1i(glGetUniformLocation(scene_models[0].getProgram(), "with_fr"),0);

				scene_models[0].draw(projectionMatrix, rotated_view);
			}
		}
#endif


        // draw the sphere
        spheremat.model_matrix = glm::translate(glm::mat4(1.0f), light0.pos);  // need to be updated in case the light moves
        sphere.draw(projectionMatrix, rotated_view, spheremat.model_matrix);


        glUseProgram(0);
}


//------------------------------------------------------------
//
//	RENDER THE SCENE


void Draw(void)
{
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 0 );

    float meanTime = 0;
    float meanHidden = 0;
    float meanVisible = 0;
    float meanSwapTime = 0;
    float meanBlitTime = 0;

    // Init the view matrix. 
    cs557::InitControlsViewMatrix(viewMatrix);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST); // ignore this line
    int count = 0;
	int mean_cout = 0;
    while (!glfwWindowShouldClose(window))
    {

        auto t0 = std::chrono::high_resolution_clock::now();

        // Clear the entire buffer with our green color (sets the background to be green).
        glClearBufferfv(GL_COLOR, 0, clear_color);
        glClearBufferfv(GL_DEPTH, 0, clear_depth);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //// This renders the objects

        // update the camera values.
        // Note that this line changes the view matrix.
        glm::mat4 rotated_view =  cs557::GetCamera().getViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) ;

        float time = glfwGetTime();
        Eigen::Vector2f pos = gaze_data.forTime(time);
       

       // cout << time << " : " << pos.x() << ", " << pos.y() << endl;



        //-----------------------------------------------------------------------------
		// 
		// Draw the hidden scene

        auto t1 = std::chrono::high_resolution_clock::now();
		

#ifdef _WITH_FR

		if (WITH_FR){

			// Bind the frame buffer object
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHidden);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// set the viewport. It must match the texture size.
			glViewport(0, 0, tex_size, tex_size);

    
			// Clear the entire buffer with our green color.
			glClearBufferfv(GL_COLOR, 0, clear_color);
			glClearBufferfv(GL_DEPTH, 0, clear_depth);


			DrawScene(false);


			// Reset to the regular buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


			glViewport(0,0, (OSZoom* window_width), (OSZoom*window_height)); // the defaut window is set to this size

			auto t1_end = std::chrono::high_resolution_clock::now();
	
			glBlitFramebuffer(0,0, tex_size, tex_size,
							0,0, OSZoom*window_width, OSZoom*window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		}

#endif
         auto t2 = std::chrono::high_resolution_clock::now();

		//-----------------------------------------------------------------------------
		//
		// Draw the scene



        // Object 0
        // This draws a coordinate system
        coordinateSystem.draw(projectionMatrix, rotated_view, modelMatrixCoord);

       // gaze_helper.bindTexture();


        DrawScene(WITH_FR);



        glUseProgram(0);

        auto t3 = std::chrono::high_resolution_clock::now();

        // Swap the buffers so that what we drew will appear on the screen.
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto t4 = std::chrono::high_resolution_clock::now();

        //---------------------------------------------------

        auto t_all = std::chrono::duration_cast<std::chrono::nanoseconds>(t4-t0);
        auto t_hidden = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1);
        auto t_visible = std::chrono::duration_cast<std::chrono::nanoseconds>(t3-t2);
        auto t_swap = std::chrono::duration_cast<std::chrono::nanoseconds>(t4-t3);
        auto t_event = std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0);

#ifdef _WITH_FR
     //   auto t_blit = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1_end);

#endif
        

      
       
      
        

          count++; // increase count before average calculation. 

        if(count > 50){
			mean_cout++;
            meanTime = meanTime * (mean_cout -1)/ mean_cout + (float(t_all.count()/1e6)) / (mean_cout );
            meanHidden = meanHidden * (mean_cout -1)/ mean_cout + (float(t_hidden.count()/1e6)) / (mean_cout );
            meanVisible = meanVisible * (mean_cout -1)/ mean_cout + (float(t_visible.count()/1e6)) / (mean_cout );
#ifdef _WITH_FR
//            meanBlitTime = meanBlitTime * (count -1)/ count + (float(t_blit.count()/1e6)) / (count );
#else
            meanBlitTime = -1;
#endif
            meanSwapTime = meanSwapTime * (mean_cout -1)/ mean_cout + (float(t_swap.count()/1e6)) / (mean_cout );

            
            
            if(count % 100 == 0)
                cout << count << ":\tmean time: " << meanTime << " ms\tmean fbo renderer: " << meanHidden << " ms\tmean visible renderer: " <<  meanVisible << " ms\tmean swap: " << meanSwapTime << " ms" << endl;
        }


		std::ofstream fout(output_file, std::ofstream::out | std::ofstream::app);
        if(count == 1) fout << "id,t_all,t_hidden,t_visible,t_swap,t_event,t_hidden + t_visible,mean_all,mean_fbo,mean_visible,mean_swap\n";
        fout << count << "," << t_all.count() <<"," << t_hidden.count() << "," << t_visible.count()  << "," << t_swap.count() << "," << t_event.count() << "," <<  t_hidden.count() + t_visible.count() << "," << meanTime << "," <<  meanHidden << ","<<  meanVisible << "," << meanSwapTime << "\n";
       
      

		if( max_iterations > 0 && count == max_iterations) {
			std::ofstream fout("result", std::ofstream::out );
			fout <<  meanTime << "\n";
			fout <<  meanSwapTime;
			fout.close();
			cout << "Write output file" << endl;
		}

        if( max_iterations > 0 && count == max_iterations) {
			exit(1);
		}
    }
}






//-w 1024 -h 1024 -tex 256 -eye 0.1 -fr -o ./data/test1.csv -end 5000

//------------------------------------------------------------
//
//	RENDER THE SCENE
int main(int argc,  char **argv)
{
    cout << "Foveated rendering test app." << endl;
    cout << "Rafael Radkowski\nrafael@iastate.edu\nIowa State University\n" << endl;
  
    cout << "Example arguments: -w 1024 -h 1024 -tex 256 -eye 0.1 -fr -o ./data/test1.csv -end 5000" << endl;



	arlab::Arguments arg = arlab::ArgParser::Parse(argc, argv);
	arlab::ArgParser::Display();

	window_height = arg.window_height;
	window_width = arg.window_width;
	OSZoom = arg.zoom;
	tex_size = arg.tex_size;
	eye_radius = arg.eye_radius;
	WITH_FR = arg.WITH_FR;
	output_file = arg.output_file;
	max_iterations = arg.end_cout;
	row_spheres = arg.row_spheres;

	//Change output file name
	int idx = output_file.find_last_of(".");
	if(idx > 0){
		output_file = output_file.substr(0,idx);
		output_file.append("_");
		output_file.append(TimeUtils::GetCurrentDateTime());
		output_file.append(".csv");
	}

	cout << "Write data to " << output_file << endl;

	std::ofstream fout(output_file, std::ofstream::out );
	fout << "FR," << WITH_FR <<"\n";
	fout << "window_height," << window_height <<"\n";
	fout << "window_width," << window_width <<"\n";
	fout << "tex_size," << tex_size <<"\n";
	fout << "FReye_radius," << eye_radius << "\n";
	fout << "row_spheres," << row_spheres << "\n";
	fout << "max_iterations," << max_iterations << "\n\n";
	fout.close();


	


    // Read eye position data
    EyeDataReader reader;
	FileUtils::Search(gaze_data_file, gaze_data_file);
    reader.read(gaze_data_file, &gaze_data);


	// Init the scene 
    Init();

	// Draw the scene
    Draw();

    return 1;
}
