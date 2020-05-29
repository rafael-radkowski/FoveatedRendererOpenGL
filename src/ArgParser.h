#ifndef __ARGPARSER__
#define __ARGPARSER__

// stl
#include <iostream>
#include <string>
#include <vector>



using namespace std;

namespace arlab
{

typedef struct _Arguments
{
	int window_height;//1200;
	int window_width;//1600;
	int zoom ;
	int tex_size ;
	float eye_radius;
	bool WITH_FR;
	int	 end_cout;
	int row_spheres;
	int num_lights;

	string output_file;
	string current_path;

	bool verbose;
	bool valid;
	

	_Arguments()
	{
		

		window_height = 1024;//1200;
		window_width = 1024;//1600;
		zoom = 1.0;
		output_file = "data.csv";
		tex_size = 128;
		row_spheres = 3;
		num_lights = 3;

		eye_radius = 0.1;
		end_cout = -1;
		WITH_FR = false;

		verbose = false;
		valid = false;
	}


}Arguments;


class ArgParser
{
public:

	/*
	Parse the arguments
	@param argc - number of arguments
	@param argv - the argument line
	@return struct with arguments
	*/
	static Arguments Parse(int& argc, char** argv);

	/*
	Display help
	*/
	static void Help(void);

	/*
	Display all arguments
	*/
	static void Display(void);

private:

	/*
	Extract the current path
	*/
	static void Path(char* path);

	/*
	Extract the current path
	*/
	static void ParamError(string option);
};



}// namespace arlab


#endif