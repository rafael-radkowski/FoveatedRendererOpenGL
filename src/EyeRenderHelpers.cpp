#include "EyeRenderHelpers.h"




GLint texture_unit = GL_TEXTURE7;

EyeRenderHelpers::EyeRenderHelpers()
{
    eye_coord = glm::vec2(0,0);
    screen_res = glm::vec2(1280,1024);
    texture_id = 0;
    radius  = 2;
    error_check = 0;
}



/*
Load the buffer pattern from a file and prepare a texture
*/
bool EyeRenderHelpers::loadBufferPattern(std::string file){
    
    // Load and create the texture
    bool ret = cs557::LoadAndCreateTexture2D(file, &texture_id, GL_CLAMP_TO_BORDER, texture_unit );

    // texture is still bound
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST );

    if(!ret)
    {
        std::cout << "Cannot load texture pattern" << endl;
    }
    return ret;
}

bool EyeRenderHelpers::setParamsToProgram(int program)
{
    glUseProgram(program);

    if(checkName(program, "screen_res")) glUniform2fv(glGetUniformLocation(program , "screen_res"), 1, &screen_res[0]);
    if(checkName(program, "eye_radius")) glUniform1f(glGetUniformLocation(program , "eye_radius"), radius);

    glActiveTexture(texture_unit);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    checkName(program, "buffer_pattern");
    int loc = glGetUniformLocation(program, "buffer_pattern");

    glUniform1i(loc, 0);


    glUseProgram(0);

    return true;
}


bool EyeRenderHelpers::bindTexture(void)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    return true;
}



bool EyeRenderHelpers::setEyeCoordinates(int program, float x, float y)
{
    eye_coord.x = x;
    eye_coord.y = y;
    glUseProgram(program);
    if(checkName(program, "eye_pos")) glUniform2fv(glGetUniformLocation(program , "eye_pos"), 1, &eye_coord[0]);

	return true;
}


bool EyeRenderHelpers::setResolution(float width, float height)
{
    screen_res.x = width;
    screen_res.y = height;

    return true;
}


bool EyeRenderHelpers::setRadius( float r)
{
    radius = r;
    return true;
}




/*
This function checks for the variable names in the shader program shader_program_id
@param program_id - the shader program id as integer.
@param name - the variable name
@return true - if the name exists, false otherwise. 
*/
bool EyeRenderHelpers::checkName(int program_id, std::string name)
{
    
    int ret = glGetUniformLocation(program_id, name.c_str());
    if(ret == -1  & error_check < 8){
        std::cout << ret << " [ERROR] - EyeRenderHelpers - Cannot find shader program variable " << name << ".\nDid you add the right variable name?" << std::endl; 
        error_check++;
        return false;
    }
    return true;
}