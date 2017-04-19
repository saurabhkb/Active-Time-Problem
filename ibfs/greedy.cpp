#include <stdio.h>
#include "atp.h"
#include "../timer/timer.h"
#include <algorithm>
#include <ctime>

vector<bool> greedy_lr(Schedule& sched) {
	vector<bool> closed;
	for(int i = 0; i < sched.num_times; i++) {
		closed.push_back(false);
	}
	for(int i = 0; i < sched.num_times; i++) {
		if(closed[i])
			continue;
		sched.close_timeslot(i);
		if(!sched.is_feasible()) {
			sched.open_timeslot(i);
		} else {
			closed[i] = true;
		}
	}
	return closed;
}


int main(int argc, char **argv) {
	// argv[1] = parallelism parameter
	// argv[2] = job data file
	if(argc != 3) {
		cout << "Invalid number of arguments!" << endl;
		return 1;
	}
	Schedule sched;
	Timer t;
	sched.read_job_data(argv[2], atoi(argv[1]));

	// obtain minimal feasible schedule and print the number of slots used
	start_timer(t);
	vector<bool> closed = greedy_lr(sched);
	stop_timer(t);

	std::cout << get_duration(t) << std::endl;

	int num_open = 0;
	for(unsigned i = 0; i < closed.size(); i++) num_open += !closed[i];
	cout << num_open << endl;

	// cout << "<SOLN>" << endl;
	// for(unsigned i = 0; i < closed.size(); i++) {
	// 	if(!closed[i]) cout << "t = " << i << endl;
	// }
	// cout << "</SOLN>" << endl;

	return 0;
}
