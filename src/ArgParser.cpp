#include "ArgParser.h"


using namespace arlab;


namespace ArgParserTypes{

	Arguments opt;
	int error_count = 0;
}

using namespace ArgParserTypes;


/*
Parse the arguments
@param argc - number of arguments
@param argv - the argument line
@return struct with arguments
*/
Arguments ArgParser::Parse(int& argc, char** argv)
{
	//cout << argc << endl;
	//cout << argv[0] << endl;

	if (argc < 2) {
		Help();
		return opt;
	}

	opt.valid = true;

	// extract the path
	Path(argv[0]);

	

	int pos = 1;
	while(pos < argc)
	{
		string c_arg(argv[pos]); 

		if (c_arg.compare("-z") == 0) { // output path
			if (argc >= pos+1) opt.zoom = atof( string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-w") == 0){ // image width 
			if (argc >= pos) opt.window_width = atoi( string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-h") == 0){ // image width 
			if (argc >= pos) opt.window_height =atoi(  string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-end") == 0){ // image width 
			if (argc >= pos) opt.end_cout =atoi(  string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-eye") == 0){ // image width 
			if (argc >= pos) opt.eye_radius =atof(  string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-o") == 0){ // image width 
			if (argc >= pos) opt.output_file =   string(argv[pos+1]).c_str() ;
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-tex") == 0){ // size of low resolution texture
			if (argc >= pos) opt.tex_size = atoi( string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-num") == 0){ // number of objects
			if (argc >= pos) opt.row_spheres = atoi( string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if (c_arg.compare("-light") == 0) { // number of lights 
			if (argc >= pos) opt.num_lights = atoi(string(argv[pos + 1]).c_str());
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-fr") == 0 || c_arg.compare("-h") == 0){ // help
			opt.WITH_FR = true;
		}
		else if(c_arg.compare("-help") == 0 || c_arg.compare("-h") == 0){ // help
			Help();
		}
		else if(c_arg.compare("-verbose") == 0 ){ // help
			opt.verbose = true;
		}
	
		// -w 1024 -h 1024 -tex 256 -eye 0.1 -fr -o ./data/test1.csv -end 5000

		pos++;
	}


	if (opt.verbose)
		Display();

	return opt;
}


/*
Display help
*/
//static
void ArgParser::Help(void)
{
/*
	cout << "[ERROR] - Missing parameters\nUsage:" << endl;
	cout << "DatasetRenderer [3d model path and filename] " << endl;
	cout << "Optional parameters:" << endl;
	cout << "\t-intr [param] - path and filename for the intrinsic camera parameters." << endl;
	cout << "\t-o [param] - set the output path" << endl;
	cout << "\t-img_w [param] \t- set the widht of the output image in pixels (integer)." << endl;
	cout << "\t-img_h [param] \t- set the height of the output image in pixels (integer)." << endl;
	cout << "\t-wnd_w [param] \t- set the widht of the application window in pixels (integer)." << endl;
	cout << "\t-wnd_h [param] \t- set the height of the application window in pixels (integer)." << endl;
	cout << "\t-m [param] \t- set the camera path models. Can be SPHERE, POLY, TREE." << endl;
	cout << "\t-seg [param] \t- for the camera path SPHERE model, set the number of segments (integer)." << endl;
	cout << "\t-rows [param] \t-for the camera path SPHERE model, set the number of rows (integer)" << endl;
	cout << "\t-dist [param] \t-for the camera path SPHERE model, set the sphere radius (float)" << endl;
	cout << "\t-sub [param] \t-for the camera path model POLY and POSE, set the number of subdivisions for the polyheder (int)" << endl;
	cout << "\t-num [param] \t- for camera path control POSE, set the number of images to generate (int)" << endl;
	cout << "\t-limx [param] \t- for camera path control POSE, set the x-axis limit for the random position (float)" << endl;
	cout << "\t-limy [param] \t- for camera path control POSE, set the y-axis limit for the random position (float)" << endl;
	cout << "\t-lim_near [param] \t- for camera path control POSE, set the near z-axis limit (positive along the camera axis) for the random position (float)" << endl;
	cout << "\t-lim_far [param] \t- for camera path control POSE, set the far z-axis limit (postive along the camera axis) for the random position (float)" << endl;
	cout << "\t-level [param] \t-for the camera path TREE, the number of tree levels for the Balanced Pose Tree (int)" << endl;
	cout << "\t-up \t- Renders objects only in the upright position if set, where up is the positive y-direction." << endl;
	cout << "\t-verbose \t- displays additional information." << endl;
	cout << "\t-help \t- displays this help menu" << endl;

	cout << "\nExample: DatasetRenderer ../data/stanford_bunny_02_rot.obj -m POLY -img_w 1280 -img_h 1024 -wnd_w 1280 - wnd_h 1024 -o output -sub=1 -rad 1.3\n" << endl;
	*/
}


/*
Display all arguments
*/
//static 
void ArgParser::Display(void)
{

		
	if(opt.WITH_FR)
		std::cout << "FOVEATED RENDERER:\t"  << endl;
	else
		std::cout << "FULL RESOLUTION RENDERER:\t"  << endl;
	std::cout << "Window width:\t" << opt.window_width << endl;
	std::cout << "Window height:\t" << opt.window_width << endl;
	std::cout << "Texture width:\t" << opt.tex_size << endl;
	std::cout << "Texture height:\t" << opt.tex_size << endl;
	std::cout << "Eye radius: \t" << opt.eye_radius << endl; 
	std::cout << "Number of Lights: \t" << opt.num_lights << endl;
	std::cout << "End at:\t" << opt.end_cout << endl;

}


/*
Extract the current path
*/
//static 
void ArgParser::Path(char* path)
{
	string str(path);

	int idx = str.find_last_of("\\");
	if(idx == -1){ 
		idx = str.find_last_of("/");
	}
	opt.current_path = str.substr(0, idx+1);
}

/*
Extract the current path
*/
//static 
void ArgParser::ParamError(string option)
{
	cout << "[ERROR] - Parameter for option " << option << " is missing or invalid." << endl;
	error_count++;
}