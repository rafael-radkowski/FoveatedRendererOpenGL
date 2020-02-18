#ifndef EYE_RENDER_HELPERS
#define EYE_RENDER_HELPERS

// stl
#include <iostream>
#include <string>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

// local
#include "Texture2D.h"

class EyeRenderHelpers
{
public:
    EyeRenderHelpers();

    /*
    Load the buffer pattern from a file and prepare a texture
    */
    bool loadBufferPattern(std::string file);

    bool bindTexture(void);

    bool setEyeCoordinates(int program, float x, float y);

    bool setParamsToProgram(int program);

    bool setResolution( float x, float y);

    bool setRadius( float radius);

private:

    /*
    This function checks for the variable names in the shader program shader_program_id
    @param program_id - the shader program id as integer.
    @param name - the variable name
    @return true - if the name exists, false otherwise. 
    */
    bool checkName(int program_id, std::string name);


    glm::vec2   eye_coord;
    glm::vec2   screen_res;
    float       radius;

    unsigned int texture_id;


    int error_check;

};



#endif