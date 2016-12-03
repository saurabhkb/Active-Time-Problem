#include "atp.h"
#include "../timer/timer.h"
#include <algorithm>
#include <ctime>

vector<bool> minimal_feasible_schedule(ATI &ati) {
	bool schedule_changed = true;
	srand(unsigned(time(0)));
	vector<int> v;
	vector<bool> closed;
	for(int i = 0; i < ati.num_times; i++) {
		v.push_back(i);
		closed.push_back(false);
	}
	while(schedule_changed) {
		schedule_changed = false;
		random_shuffle(v.begin(), v.end());
		for(int i = 0; i < ati.num_times; i++) {
			if(closed[i])
				continue;
			close_timeslot(ati, i);
			if(!feasible_schedule_exists(ati)) {
				open_timeslot(ati, i);
			} else {
				closed[i] = true;
				schedule_changed = true;
			}
		}
	}
	return closed;
}


int main() {
	ATI ati;
	Timer t;
	read_jobdata_stdin(ati);

	start_timer(t);
	vector<bool> closed = minimal_feasible_schedule(ati);
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

