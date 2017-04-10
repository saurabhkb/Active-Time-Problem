#include "atp.h"

Schedule::Schedule() {
	rev = get(boost::edge_reverse, g);
	capacity_map = get(boost::edge_capacity, g);

	// initialize source and sink
	src = boost::add_vertex(g);
	g[src].type = SRC;
	sink = boost::add_vertex(g);
	g[sink].type = SINK;

	// clear the arrays holding job and time nodes
	memset(jobnodes, 0, MAXJOBS * sizeof(int));
	memset(timenodes, 0, MAXTIMES * sizeof(int));

	num_jobs = 0;
	num_times = 0;
}

// helper method to create edge between v1 and v2
Edesc Schedule::add_edge_wrapper(Vdesc &v1, Vdesc &v2, double capacity) {
	Edesc e1 = boost::add_edge(v1, v2, g).first;
	Edesc e2 = boost::add_edge(v2, v1, g).first;
	put(boost::edge_capacity, g, e1, capacity);
	put(boost::edge_capacity, g, e2, 0);

	rev[e1] = e2;
	rev[e2] = e1;

	return e1;
}

int Schedule::get_num_open_timeslots() {
	int count = 0;
	for(int i = 0; i <= num_times; i++) {
		if(!g[timenodes[i]].closed) count++;
	}
	return count;
}

bool Schedule::is_closed(int t) {
	return g[timenodes[t]].closed;
}

void Schedule::read_job_data() {
	int r, d, p;
	std::cin >> cap;
	while(std::cin >> r >> d >> p) {
		assert(num_jobs < MAXJOBS);
		jobnodes[num_jobs] = boost::add_vertex(g);
		g[jobnodes[num_jobs]].jobid = num_jobs;
		g[jobnodes[num_jobs]].r = r;
		g[jobnodes[num_jobs]].d = d;
		g[jobnodes[num_jobs]].p = p;
		g[jobnodes[num_jobs]].type = JOB;
		num_jobs++;
		processing_time_sum += p;
		if(d > num_times) num_times = d;
	}

	// add all time slots in range [0, num_times]
	for(int i = 0; i <= num_times; i++) {
		timenodes[i] = boost::add_vertex(g);
		g[timenodes[i]].t = i;
		g[timenodes[i]].closed = false;
		g[timenodes[i]].type = TIME;
	}

	// add time nodes and parallel parameter (B or g) capacity edges to sink
	for(int t = 0; t <= num_times; t++) {
		add_edge_wrapper(timenodes[t], sink, cap);
	}

	// add job related edges
	for(int i = 0; i < num_jobs; i++) {
		// add src to job edges with capacity = processing time of job
		add_edge_wrapper(src, jobnodes[i], g[jobnodes[i]].p);
		// add unit capacity edges between job nodes and time nodes
		for(int t = g[jobnodes[i]].r; t <= g[jobnodes[i]].d; t++) {
			add_edge_wrapper(jobnodes[i], timenodes[t], 1);
		}
	}

}

bool Schedule::is_feasible() {
	return processing_time_sum <= push_relabel_max_flow(g, src, sink);
}

void Schedule::close_timeslot(int t) {
	Vdesc v = timenodes[t];
	g[v].closed = true;
	std::pair<Edesc, bool> todelete, todelete_rev;

	todelete = edge(v, sink, g);
	todelete_rev = edge(sink, v, g);

	put(boost::edge_capacity, g, todelete.first, 0);
	put(boost::edge_capacity, g, todelete_rev.first, 0);
}

void Schedule::open_timeslot(int t) {
	Vdesc v = timenodes[t];
	g[v].closed = false;
	std::pair<Edesc, bool> torestore, torestore_rev;

	torestore = edge(v, sink, g);
	torestore_rev = edge(sink, v, g);
	put(boost::edge_capacity, g, torestore.first, cap);
	put(boost::edge_capacity, g, torestore_rev.first, 0);
}
