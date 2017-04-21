#include <stdio.h>
#include "atp.h"
#include "../timer/timer.h"
#include <algorithm>
#include <set>
#include <ctime>
#include <fstream>
#include <sstream>

// Converts an int to a string.
string to_string(int& n) {
	ostringstream os;
	os << n;
	return os.str();
}

// Returns a string in which the ints in the vector have been concatenated.
// The ints are separated by the string argument joiner.
string join(vector<int> v, string joiner = ",") {
	string s = "";
	for(unsigned i = 0; i < v.size(); i++) {
		s += to_string(v[i]);
		if(i < v.size() - 1) s += joiner;
	}
	return s;
}

// Helper recursive method for generating combinations.
void combinations(vector<vector<int> >& allcombos, vector<int> tocombine, vector<int> combo, int offset, int k) {
	if(k == 0) {
		// found a combination
		allcombos.push_back(combo);
		return;
	}
	for(unsigned i = offset; i <= tocombine.size() - k; i++) {
		combo.push_back(tocombine[i]);
		combinations(allcombos, tocombine, combo, i + 1, k - 1);
		combo.pop_back();
	}
}

// Returns a vector of all k-combinations of elements in tocombine.
vector<vector<int> > generate_combinations(vector<int> tocombine, int k) {
	vector<vector<int> > allcombos;
	vector<int> combo;
	combinations(allcombos, tocombine, combo, 0, k);
	return allcombos;
}

// Reads a schedule file and modifies sched to reflect the schedule that was read.
void read_schedule(Schedule& sched, char *schedfile) {
	// close all timeslots
	for(int i = 0; i < sched.num_times; i++) {
		sched.close_timeslot(i);
	}
	ifstream infile(schedfile);
	string line;

	// open only the ones specified in the given schedule
	while(getline(infile, line)) {
		sched.open_timeslot(atoi(line.c_str()));
	}
}

void localsearch(Schedule& sched, int b) {
	bool schedule_changed = true;
	set<int> newly_opened_slots;

	while(schedule_changed) {
		// New iteration:
		// find set of empty and active slots
		vector<int> empty, active;
		for(int i = 0; i < sched.num_times; i++) {
			if(!sched.is_timeslot_active(i)) {
				empty.push_back(i);
			} else if(newly_opened_slots.count(i) == 0) {
				active.push_back(i);
			}
		}

		if(empty.size() == 0 || active.size() == 0) break;

		schedule_changed = false;

		vector<vector<int> > to_open_combos, to_close_combos;
		to_open_combos = generate_combinations(empty, b);
		to_close_combos = generate_combinations(active, b + 1);

		for(unsigned i = 0; i < to_open_combos.size(); i++) {
			for(unsigned j = 0; j < to_close_combos.size(); j++) {
				// open and close the appropriate time slots
				for(unsigned k = 0; k < to_open_combos[i].size(); k++)
					sched.open_timeslot(to_open_combos[i][k]);

				for(unsigned k = 0; k < to_close_combos[j].size(); k++)
					sched.close_timeslot(to_close_combos[j][k]);

				if(!sched.is_feasible()) {
					// undo the operations
					for(unsigned k = 0; k < to_open_combos[i].size(); k++)
						sched.close_timeslot(to_open_combos[i][k]);

					for(unsigned k = 0; k < to_close_combos[j].size(); k++)
						sched.open_timeslot(to_close_combos[j][k]);
				} else {
					// otherwise break, so that we can keep this change
					// and recompute the empty and active slots for the next iteration
					cout << "opened " << join(to_open_combos[i]);
					cout << " and closed " << join(to_close_combos[j]) << endl;

					// add the opened slots to the newly_opened_slots set so that those
					// slots aren't closed again.
					for(unsigned k = 0; k < to_open_combos[i].size(); k++) {
						newly_opened_slots.insert(to_open_combos[i][k]);
					}
					// TODO now that we know that at least b + 1 slots can be closed, try to find
					// minimal feasible solution here.
					schedule_changed = true;
					break;
				}
			}
			// if the previous loop succeeded, break out of this one too to start next iteration
			if(schedule_changed) break;
		}
	}
}

int main(int argc, char **argv) {
	// argv[1] = parallelism parameter
	// argv[2] = job file
	// argv[3] = schedule file
	if(argc != 4) {
		cout << "Invalid number of arguments" << endl;
		return 1;
	}
	Schedule sched;
	Timer t;
	sched.read_job_data(argv[2], atoi(argv[1]));
	read_schedule(sched, argv[3]);
	if(!sched.is_feasible()) {
		cout << "Schedule is not feasible!" << endl;
		return 1;
	}

	start_timer(t);
	localsearch(sched, 1);
	stop_timer(t);

	int soln = 0;
	for(int t = 0; t < sched.num_times; t++) soln += sched.is_timeslot_active(t);

	cout << get_duration(t) << endl;
	cout << soln << endl;

	sched.print_job_assignment();
}
