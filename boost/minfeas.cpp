#include "atp.h"
#include <vector>
#include <algorithm>

void minimal_feasible_schedule(ATI &ati) {
	bool schedule_changed = true;
	std::srand(unsigned(std::time(0)));
	std::vector<int> v;
	for(int i = 0; i <= ati.num_times; i++) {
		if(!ati.g[ati.timenodes[i]].closed) v.push_back(i);
	}
	while(schedule_changed) {
		schedule_changed = false;
		std::random_shuffle(v.begin(), v.end());
		for(int i = 0; i <= ati.num_times; i++) {
			if(ati.g[ati.timenodes[v[i]]].closed)
				continue;
			close_timeslot(ati, ati.timenodes[v[i]]);
			if(!feasible_schedule_exists(ati)) {
				open_timeslot(ati, ati.timenodes[v[i]]);
			} else {
				ati.g[ati.timenodes[v[i]]].closed = true;
				schedule_changed = true;
			}
		}
	}
}


void read_jobdata_stdin(ATI &ati) {
	int r, d, p;
	std::cin >> ati.cap;
	while(std::cin >> r >> d >> p)
		add_job(ati, r, d, p);
}

void create_tight_3example(ATI &ati, int g) {
	/* g-2 rigid jobs of length g in range [x, x + g)
	g unit jobs in range [x, x + g - 1)
	g unit jobs in range (x, x + g - 1]
	one job of length g in range [x - g, x + g - 1]
	one job of length g in range [x, x + 2g] */

	int x = g;	// start at 0
	for(int i = 0; i < g - 2; i++) {
		add_job(ati, x, x + g - 1, g);
	}
	for(int i = 0; i < g; i++) {
		add_job(ati, x, x + g - 1, 1);
		add_job(ati, x + 1, x + g, 1);
	}
	add_job(ati, x - g, x + g - 1, g);
	add_job(ati, x, x + 2 * g, g);
}


int main(int argc, char **argv) {
	ATI ati;

	Timer t;

	ati_init(ati);
	read_jobdata_stdin(ati);
	ati_jobadd_complete(ati);

	// obtain minimal feasible schedule and print the number of slots used
	start_timer(t);
	minimal_feasible_schedule(ati);
	stop_timer(t);

	std::cout << get_duration(t) << std::endl;

	int soln = get_num_open_timeslots(ati);
	std::cout << soln << std::endl;

	return 0;
}

