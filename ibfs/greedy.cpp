#include "atp.h"
#include "../timer/timer.h"
#include <algorithm>
#include <ctime>

vector<bool> greedy_lr(ATI &ati) {
	vector<bool> closed;
	for(int i = 0; i < ati.num_times; i++) {
		closed.push_back(false);
	}
	for(int i = 0; i < ati.num_times; i++) {
		if(closed[i])
			continue;
		close_timeslot(ati, i);
		if(!feasible_schedule_exists(ati)) {
			open_timeslot(ati, i);
		} else {
			closed[i] = true;
		}
	}
	return closed;
}


int main(int argc, char **argv) {
	ATI ati;

	Timer t;

	read_jobdata_stdin(ati);

	// obtain minimal feasible schedule and print the number of slots used
	start_timer(t);
	vector<bool> closed = greedy_lr(ati);
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
