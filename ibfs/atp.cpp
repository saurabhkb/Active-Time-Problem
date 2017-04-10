#include "atp.h"
#include "../timer/timer.h"

void Schedule::read_job_data() {
	int r, d, p;
	int max_deadline = -1, num_edges = 0;
	vector<int> rvec, dvec, pvec;

	processing_time_sum = 0;

	// read job data
	cin >> cap;
	while(cin >> r >> d >> p) {
		rvec.push_back(r);
		dvec.push_back(d);
		pvec.push_back(p);
		if(max_deadline < d) max_deadline = d;
		num_edges += d - r + 1;
	}

	// initialize private variables
	num_jobs = rvec.size();
	num_times = max_deadline + 1;

	// initialize internal graph representation
	Graph = new IBFSGraph(IBFSGraph::IB_INIT_FAST);

	// number of edges does not include edges from the source and to the sink
	// there will be one edge between the job and time nodes per time unit that the job is alive
	Graph->initSize(num_jobs + num_times, num_edges);

	// add time nodes with capacity g
	for(int i = 0; i < num_times; i++) {
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
	Graph->incNode(num_jobs + t, 0, -cap);
}

void Schedule::open_timeslot(int t) {
	Graph->incNode(num_jobs + t, 0, cap);
}

bool Schedule::is_feasible() {
	Graph->computeMaxFlow(true);
	return Graph->getFlow() == processing_time_sum;
}
