#pragma once

// stl include
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <vector>


class TimeMeasurement
{
public:

	/*
	Init a number of time points
	*/
	static bool Init(int num_time_points, std::vector<std::string> time_point_labels);

	/*
	*/
	static bool Record(int time_point_index);


	/*
	*/
	static bool CalcMeanTimes(void);



	static void AddTimeDiff(int start_index, int stop_index, std::string label);


	static void SetSaveFile(std::string path_and_file);


	static bool Write(void);


	static float GetMeanTime(int index);



	static void Display(void);

};
