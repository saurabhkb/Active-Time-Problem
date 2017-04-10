#include "atp.h"
#include <vector>
#include <algorithm>

void minimal_feasible_schedule(Schedule& sched) {
	bool schedule_changed = true;
	std::srand(unsigned(std::time(0)));
	std::vector<int> v;
	for(int i = 0; i <= sched.num_times; i++) {
		if(!sched.is_closed(i)) v.push_back(i);
	}
	while(schedule_changed) {
		schedule_changed = false;
		std::random_shuffle(v.begin(), v.end());
		for(int i = 0; i <= sched.num_times; i++) {
			if(sched.is_closed(v[i])) continue;

			sched.close_timeslot(v[i]);
			if(!sched.is_feasible()) sched.open_timeslot(v[i]);
			else schedule_changed = true;
		}
	}
}


// void create_tight_3example(ATI &ati, int g) {
// 	/* g-2 rigid jobs of length g in range [x, x + g)
// 	g unit jobs in range [x, x + g - 1)
// 	g unit jobs in range (x, x + g - 1]
// 	one job of length g in range [x - g, x + g - 1]
// 	one job of length g in range [x, x + 2g] */
// 
// 	int x = g;	// start at 0
// 	for(int i = 0; i < g - 2; i++) {
// 		add_job(ati, x, x + g - 1, g);
// 	}
// 	for(int i = 0; i < g; i++) {
// 		add_job(ati, x, x + g - 1, 1);
// 		add_job(ati, x + 1, x + g, 1);
// 	}
// 	add_job(ati, x - g, x + g - 1, g);
// 	add_job(ati, x, x + 2 * g, g);
// }


int main(int argc, char **argv) {
	Schedule sched;

	Timer t;

	sched.read_job_data();

	// obtain minimal feasible schedule and print the number of slots used
	start_timer(t);
	minimal_feasible_schedule(sched);
	stop_timer(t);

	std::cout << get_duration(t) << std::endl;

	std::cout << sched.get_num_open_timeslots() << std::endl;

	return 0;
}

