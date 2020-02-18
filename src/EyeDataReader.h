/*
UTDataReader

This class implements an abstract data reader.
It just opens an ascci file and provides the data line by line to an abstract function 
that implements the data prototcol, requires a derived class



Rafael Radkowski
Iowa State University
Aug. 2018
All copyrights reserved.
*/
#pragma once

// stl
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <strstream>
#include <sstream>
#include <assert.h>

//local
#include "EyeData.h"
#include "Exists.h"

using namespace std;

class EyeDataReader
{
public:

	// Constructor and destructor
	EyeDataReader();
	~EyeDataReader();


	/*
	Read data from an ASCII file
	@param path_and_filename - string containing the relative or absolute path to the file.
	@parma dataset - Address to the storage for the dataset
	@return 1 - if file is available. Otherwise -1;
	*/
	int read(string path_and_filename, EyeData* dataset);

protected:

	/*
	Function to process the line. Needs to be implemented when a class implements a data protocol.
	@param line - the current line.
	@param dataset - the dataset to write into.
	@return 1 - if successfull.
	*/
	int process_line(string& line, EyeData* dataset);



	/*
	Check whether the filetype meets the expected one. 
	@param path_and_filename - string containing the relative or absolute path to the file.
	@return true - if the filename is correct. Otherwise false. 
	*/
	bool check_filetype(string path_and_filename);


	/*
	Split a line by commas and return the values as vector.
	@param line - the line as string
	@param output - the separated values as vector.
	*/
	int split_by_comma(string& line, vector<string>& output);



protected:

	//--------------------------------------------------------
	// data


	int			_linecount;

};