#include <stdio.h>
#include "atp.h"
#include "../timer/timer.h"
#include <algorithm>
#include <ctime>

vector<bool> minimal_feasible_schedule(Schedule& sched) {
	bool schedule_changed = true;
	srand(unsigned(time(0)));
	vector<int> v;
	vector<bool> closed;
	for(int i = 0; i < sched.num_times; i++) {
		v.push_back(i);
		closed.push_back(false);
	}
	while(schedule_changed) {
		schedule_changed = false;
		random_shuffle(v.begin(), v.end());
		for(int i = 0; i < sched.num_times; i++) {
			if(closed[v[i]])
				continue;
			sched.close_timeslot(v[i]);
			if(!sched.is_feasible()) {
				sched.open_timeslot(v[i]);
			} else {
				closed[v[i]] = true;
				schedule_changed = true;
			}
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

	start_timer(t);
	vector<bool> closed = minimal_feasible_schedule(sched);
	stop_timer(t);

	cout << get_duration(t) << endl;

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

