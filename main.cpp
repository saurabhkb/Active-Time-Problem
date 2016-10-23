#include "atp.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

typedef std::vector<int> combination_t;

typedef struct {
	int N, R, generated;
	combination_t curr;
	bool completed;
} Combinations;

void init_combinations(Combinations &c, int N, int R) {
	c.N = N;
	c.R = R;
	c.completed = N < 1 || R > N;
	c.generated = 0;
	for(int i = 0; i < R; ++i)
		c.curr.push_back(i);
}

// http://stackoverflow.com/questions/9430568/generating-combinations-in-c
combination_t next(Combinations &c) {
	combination_t ret = c.curr;

	// find what to increment
	c.completed = true;
	for (int i = c.R - 1; i >= 0; --i) {
		if (c.curr[i] < c.N - c.R + i + 1) {
			int j = c.curr[i] + 1;
			while (i <= c.R)
				c.curr[i++] = j++;
			c.completed = false;
			++c.generated;
			break;
		}
	}
	return ret;
}

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
			// std::cout << "trying to close " << ati.g[ati.timenodes[v[i]]].t << std::endl;
			close_timeslot(ati, ati.timenodes[v[i]]);
			if(!feasible_schedule_exists(ati)) {
				// std::cout << "could not close " << ati.g[ati.timenodes[v[i]]].t << std::endl;
				open_timeslot(ati, ati.timenodes[v[i]]);
			} else {
				ati.g[ati.timenodes[v[i]]].closed = true;
				schedule_changed = true;
			}
		}
	}
}


void greedy_lr(ATI &ati) {
	bool schedule_changed = true;
	while(schedule_changed) {
		schedule_changed = false;
		for(int i = 0; i <= ati.num_times; i++) {
			if(ati.g[ati.timenodes[i]].closed)
				continue;
			close_timeslot(ati, ati.timenodes[i]);
			if(!feasible_schedule_exists(ati)) {
				open_timeslot(ati, ati.timenodes[i]);
			} else {
				ati.g[ati.timenodes[i]].closed = true;
				schedule_changed = true;
			}
		}
	}
}

// close 2, open 1
void localsearch2(ATI &ati) {
	bool schedule_changed = true;
	Combinations cs;
	combination_t c;
	init_combinations(cs, ati.num_times, 2);

	while(schedule_changed) {
		schedule_changed = false;

		for(int i = 0; i <= ati.num_times; i++) {
			// try opening this one and closing two others
			if(!ati.g[ati.timenodes[i]].closed) continue;

			// restore_edge(ati, ati.timenodes[i], ati.sink, MAXCAP);
			while(!cs.completed) {
				c = next(cs);
				std::cout << c[0] << ", " << c[1] << std::endl;
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

	ati_init(ati);
	read_jobdata_stdin(ati);
	ati_jobadd_complete(ati);

	details(ati);

	// print_all_timeslots(ati);
	if(!feasible_schedule_exists(ati)) {
		std::cout << "instance is infeasible!" << std::endl;
		return 0;
	} else {
		std::cout << "instance is feasible" << std::endl;
	}
	// obtain minimal feasible schedule
	minimal_feasible_schedule(ati);
	// greedy_lr(ati);
	print_num_open_timeslots(ati);

	// print_open_timeslots(ati);

	return 0;
}

