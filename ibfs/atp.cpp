#include "atp.h"
#include "../timer/timer.h"
#include <fstream>

void Schedule::read_job_data(char *jfile, int B) {
	string j;
	int r, d, p;
	int max_deadline = -1, num_edges = 0;
	vector<int> rvec, dvec, pvec;

	processing_time_sum = 0;
	cap = B;

	// read job data
	ifstream infile(jfile);
	while(infile >> j >> r >> d >> p) {
		rvec.push_back(r);
		dvec.push_back(d);
		pvec.push_back(p);
		if(max_deadline < d) max_deadline = d;
		num_edges += d - r + 1;
	}

	// initialize private variables
	num_jobs = rvec.size();
	num_times = max_deadline + 1;

	// initialize the open/close indicator vector
	active.clear();
	for(int i = 0; i < num_times; i++)
		active.push_back(true);

	// initialize internal graph representation
	Graph = new IBFSGraph(IBFSGraph::IB_INIT_FAST);

	// number of edges does not include edges from the source and to the sink
	// there will be one edge between the job and time nodes per time unit that the job is alive
	Graph->initSize(num_jobs + num_times, num_edges);

	// add time nodes with capacity g
	for(int i = 0; i < num_times; i++) {
		// all slots are initially open
		Graph->addNode(num_jobs + i, 0, cap);
	}

	// add job nodes
	for(int i = 0; i < num_jobs; i++) {
		Graph->addNode(i, pvec[i], 0);
		processing_time_sum += pvec[i];
		for(int j = rvec[i]; j <= dvec[i]; j++) {
			Graph->addEdge(i, num_jobs + j, 1, 0);
		}
	}

	Graph->initGraph();
	Graph->computeMaxFlow(true);	// compute initial flow (need this or it will segfault)
}

void Schedule::close_timeslot(int t) {
	// close timeslot by decrementing capacity only if not already closed
	if(!active[t]) return;
	Graph->incNode(num_jobs + t, 0, -cap);
	active[t] = false;
}

void Schedule::open_timeslot(int t) {
	// open timeslot by incrementing capacity only if not already open
	if(active[t]) return;
	Graph->incNode(num_jobs + t, 0, cap);
	active[t] = true;
}

bool Schedule::is_timeslot_active(int t) {
	return active[t];
}

bool Schedule::is_feasible() {
	Graph->computeMaxFlow(true);
	return Graph->getFlow() == processing_time_sum;
}
