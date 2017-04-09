#include "atp.h"
#include "../timer/timer.h"

void details(ATI &ati) {
	cout << "num_jobs = " << ati.num_jobs << endl;
	cout << "num_times = " << ati.num_times << endl;
	cout << "processing_time_sum = " << ati.processing_time_sum << endl;
}

void ati_init(ATI &ati, int num_jobs, int num_times, int m) {
	ati.num_jobs = num_jobs;
	ati.num_times = num_times;
	ati.processing_time_sum = 0;
	ati.Graph = new IBFSGraph(IBFSGraph::IB_INIT_FAST);
	ati.Graph->initSize(num_jobs + num_times, m);

	// add time nodes with capacity g
	for(int i = 0; i < num_times; i++) {
		ati.Graph->addNode(num_jobs + i, 0, ati.cap);
	}
}

void add_jobs(ATI &ati, vector<int> rvec, vector<int> dvec, vector<int> pvec) {
	for(int i = 0; i < ati.num_jobs; i++) {
		ati.Graph->addNode(i, pvec[i], 0);
		ati.processing_time_sum += pvec[i];
		for(int j = rvec[i]; j <= dvec[i]; j++) {
			ati.Graph->addEdge(i, ati.num_jobs + j, 1, 0);
		}
	}
	ati.Graph->initGraph();
	ati.Graph->computeMaxFlow(true);	// compute initial flow (need this or it will segfault)
}

void close_timeslot(ATI &ati, int t) {
	ati.Graph->incNode(ati.num_jobs + t, 0, -ati.cap);
}

void open_timeslot(ATI &ati, int t) {
	ati.Graph->incNode(ati.num_jobs + t, 0, ati.cap);
}

bool feasible_schedule_exists(ATI &ati) {
	ati.Graph->computeMaxFlow(true);
	return ati.Graph->getFlow() == ati.processing_time_sum;
}


void read_jobdata_stdin(ATI &ati) {
	int r, d, p;
	int m = 0, max_deadline = -1;
	vector<int> rvec, dvec, pvec;
	std::cin >> ati.cap;
	while(std::cin >> r >> d >> p) {
		rvec.push_back(r);
		dvec.push_back(d);
		pvec.push_back(p);
		if(max_deadline < d) max_deadline = d;
		m += d - r + 1;
	}
	ati_init(ati, rvec.size(), max_deadline + 1, m);
	add_jobs(ati, rvec, dvec, pvec);
}
