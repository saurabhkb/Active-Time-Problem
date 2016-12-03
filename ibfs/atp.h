#include <iostream>
#include <vector>
#include <stdlib.h>
#include "ibfs.h"

using namespace std;

typedef struct {
	IBFSGraph *Graph;
	int cap;
	int num_jobs;
	int num_times;
	int processing_time_sum;
} ATI;


void ati_init(ATI &, int, int, int);
void add_jobs(ATI &, vector<int>, vector<int>, vector<int>);
void close_timeslot(ATI &, int);
void open_timeslot(ATI &, int);
bool feasible_schedule_exists(ATI &);
void read_jobdata_stdin(ATI &);
