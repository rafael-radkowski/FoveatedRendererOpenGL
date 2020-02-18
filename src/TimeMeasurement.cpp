#include "TimeMeasurement.h"



namespace ns_TimeMeasurement {

	std::vector< std::chrono::time_point<std::chrono::high_resolution_clock> > time_points;
	size_t time_points_size = 0;
	std::vector<std::string>  g_time_point_labels;
	
	std::vector<std::pair<int, int> > delta_time;
	std::vector< float > mean_time;
	std::vector<double>	measurements;
	std::vector< std::string > mean_time_label;

	std::string _path_and_file = "data.csv";


	int count = 0;
};


using namespace ns_TimeMeasurement;



/*
Init a number of time points
*/
bool TimeMeasurement::Init(int num_time_points, std::vector<std::string> time_point_labels)
{
	time_points.clear();

	for (int i = 0; i < num_time_points; i++) {
		time_points.push_back( std::chrono::time_point<std::chrono::high_resolution_clock>()  );
		
	}
	time_points_size = time_points.size();
	count = 0;

	g_time_point_labels = time_point_labels;

	return true;
}





/*
*/
bool TimeMeasurement::Record(int time_point_index)
{
	if (time_point_index >= 0 && time_point_index < time_points_size) {
		time_points[time_point_index] = std::chrono::high_resolution_clock::now();
		return true;
	}
	
	

	return false;
}



/*
*/
bool TimeMeasurement::CalcMeanTimes()
{
	count++;

	for (int i = 0; i < delta_time.size(); i++) {
		 int idx0 = delta_time[i].first;
		 int idx1 = delta_time[i].second;

		 auto t_delta = std::chrono::duration_cast<std::chrono::nanoseconds>(time_points[idx1]-time_points[idx0]);
		 float t_delta_float = float(t_delta.count() / 1e6);
		 measurements[i] = t_delta_float;

		 mean_time[i] =  mean_time[i] * (count -1)/ count + (t_delta_float/ count );
	}

	return true;
}




//static 
void TimeMeasurement::AddTimeDiff(int start_index, int stop_index, std::string label)
{
	if (start_index >= 0 && start_index  < time_points_size && stop_index >= 0 && stop_index  < time_points_size ) {
		delta_time.push_back(std::make_pair(start_index, stop_index));
		mean_time.push_back(0.0);
		mean_time_label.push_back(label);
		measurements.push_back(0.0);
	}
}


//static 
void TimeMeasurement::SetSaveFile(std::string path_and_file)
{
	_path_and_file = path_and_file;
}


//static 
bool TimeMeasurement::Write(void)
{
	

	if (count == 1) {
		
		std::ofstream off(_path_and_file, std::ofstream::out | std::ofstream::app );

		if (!off.is_open()) {
			return false;
		}
		
		off << "idx";

		for( auto l : g_time_point_labels){
			off << ", " << l;
		}
		for( auto l : mean_time_label){
			l.append("_diff");
			off << ", " << l;
		}
		for( auto l : mean_time_label){
			l.append("_mean");
			off << ", " << l;
		}
		off << "\n";

		off.close();
	}

	std::ofstream off(_path_and_file, std::ofstream::out |  std::ofstream::app);

	if (!off.is_open()) {
		return false;
	}
	
	off << count ;

	for( auto l : time_points){
		off << ", " <<  l.time_since_epoch().count();
	}

	for( auto l :  measurements){
		off << ", " <<  l;
	}


	for( auto l :  mean_time){
		off << ", " <<  l;
	}
	off << "\n";

	off.close();

	return true;
}


//static 
float TimeMeasurement::GetMeanTime(int index)
{
	return mean_time[index];
}


//static 
void TimeMeasurement::Display(void)
{
	if (count % 100 == 0) {
		std::cout << count << "\t";
		for( auto l :  mean_time){
			std::cout <<  l << "\t" ;
		}
		std::cout << "\n" ;
	}

}