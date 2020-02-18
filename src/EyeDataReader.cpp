#include "EyeDataReader.h"


// Constructor and destructor
EyeDataReader::EyeDataReader()
{
	_linecount = 0;
}


EyeDataReader::~EyeDataReader()
{

}


/*
Read UT data from an ASCII file
@param path_and_filename - string containing the relative or absolute path to the file.
@parma dataset - Address to the storage for the dataset
@return 1 - if file is available. Otherwise -1;
*/
//static 
int EyeDataReader::read(string path_and_filename, EyeData* dataset)
{

	assert(dataset != NULL);

	if (Exists(path_and_filename) == false)
	{
		std::cout << "[ERROR] - cannot find the file " << path_and_filename << std::endl;
		return -1;
	}

	if (check_filetype(path_and_filename) == false)
	{
		std::cout << "[ERROR] - [INFO] - Different filetype expected, got  " << path_and_filename << std::endl;
		return -1;
	}

	std::ifstream in(path_and_filename, std::ofstream::in);

	if (!in.is_open())
	{
		std::cout << "\n[ERROR] - cannot open the file " << path_and_filename  << std::endl;
		return -1;
	}





	//--------------------------------------------------------------------
	// process the data

	_linecount = 0;
	dataset->clear();
	dataset->filename = path_and_filename;

	string line;

	// first line
	getline(in, line);
	if( line.compare("EYE_POSITION_DATA") != 0)
	{
		std::cout << "\n[ERROR] - incorrect data in " << path_and_filename  << std::endl;
		return -1;
	}

	// framerate
	getline(in, line);
	dataset->framerate = atof(line.c_str());

	// header
	getline(in, line);
	
	// data
	while (getline(in, line))
	{
		process_line(line, dataset);
		_linecount++;
	}
	
	in.close();

	std::cout << "\n[INFO] - read " << dataset->position.size() << " lines of data from file " << path_and_filename  << std::endl;

	return 1;
}



/*
Function to process the line. Needs to be implemented when a class implements a data protocol.
@param line - the current line.
@param dataset - the dataset to write into.
@return 1 - if successfull.
*/
int EyeDataReader::process_line(string& line, EyeData* dataset)
{
	if(line.size() < 10  ) return 0;
	vector<string> elements;
	split_by_comma(line, elements);

	if(elements.size() == 3){
		dataset->position.push_back( Eigen::Vector2f( atof(elements[1].c_str()), atof(elements[2].c_str()) ));
		dataset->index.push_back(int(atoi(elements[0].c_str())));
	}
	return 1;
}



/*
Check whether the filetype meets the expected one. 
@param path_and_filename - string containing the relative or absolute path to the file.
@return true - if the filename is correct. Otherwise false. 
*/
bool EyeDataReader::check_filetype(string path_and_filename)
{

	return 1;
}




/*
Split a line by commas and return the values as vector.
@param line - the line as string
@param output - the separated values as vector.
*/
int EyeDataReader::split_by_comma(string& line, vector<string>& output)
{

	output.clear();
	std::stringstream ss(line);

	while (ss.good())
	{
		string substr;
		getline(ss, substr, ',');
		output.push_back(substr);
	}

	return 1;
}