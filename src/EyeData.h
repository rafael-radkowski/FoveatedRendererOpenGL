#ifndef _EYEDATA
#define _EYEDATA

// stl
#include <iostream>
#include <vector>

// eigen
#include <Eigen/Dense>

using namespace std;

typedef struct _EyeData
{
public:

	vector<int>					index;
	vector<Eigen::Vector2f>		position;

	string 						filename;
	float						framerate;

	/*
	Clears the data.
	*/
	void clear(void) {
		index.clear();
		position.clear();
	};


	/* Return the size of the dataset */
	int size(void)
	{
		return position.size();
	}

	/*
	Return the duration of the eye tracking data. 
	 */
	float duration(void)
	{
		return position.size() / framerate;
	}



	/*
	Return a value at a certain time
	*/
	Eigen::Vector2f get(int index){
		if (index > position.size() || index < 0) return Eigen::Vector2f(-1, -1);
		return position[index];
	}

	/*
	Return the eye point values for a certain point in time.
	*/
	Eigen::Vector2f forTime(float time){
		const float d = duration();
		float ct = time - int(time/d) * d;
		int ct_idx = int(ct * framerate);

		if(ct_idx < position.size() )
			return position[ct_idx];
		else
			return position[0];

	}


}EyeData;



#endif