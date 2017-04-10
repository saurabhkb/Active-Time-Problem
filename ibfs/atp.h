#include <iostream>
#include <vector>
#include <stdlib.h>
#include "ibfs.h"

using namespace std;

class Schedule {
public:
	int num_times;
	int num_jobs;
	int cap;
	int processing_time_sum;

	void read_job_data();
	void close_timeslot(int);
	void open_timeslot(int);
	bool is_feasible();

private:
	IBFSGraph *Graph;
};

