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

	void read_job_data(char *, int);
	void close_timeslot(int);
	void open_timeslot(int);
	bool is_timeslot_active(int);
	bool is_feasible();
	bool is_job_assigned(int, int);
	void print_job_assignment();

private:
	IBFSGraph *Graph;
	vector<bool> active;

	vector<string> jv;
	vector<int> rv, dv, pv;
};

