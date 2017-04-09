#include "atp.h"

void construct_edges(ATI &);
void add_time_vertices(ATI &, int);

void details(ATI &ati) {
	std::cout << "#jobs = " << ati.num_jobs << std::endl;
	std::cout << "#times = " << ati.num_times << std::endl;
}

// helper method to create edge between v1 and v2
Edesc AddEdge(ATI &ati, Vdesc &v1, Vdesc &v2, double capacity) {
	Edesc e1 = add_edge(v1, v2, ati.g).first;
	Edesc e2 = add_edge(v2, v1, ati.g).first;
	put(edge_capacity, ati.g, e1, capacity);
	put(edge_capacity, ati.g, e2, 0);

	ati.rev[e1] = e2;
	ati.rev[e2] = e1;

	return e1;
}

int get_num_open_timeslots(ATI &ati) {
	int count = 0;
	for(int i = 0; i <= ati.num_times; i++) {
		if(!ati.g[ati.timenodes[i]].closed) count++;
	}
	return count;
}

void print_open_timeslots(ATI &ati) {
	int count = 0;
	for(int i = 0; i <= ati.num_times; i++) {
		if(!ati.g[ati.timenodes[i]].closed) count++;
	}
	std::cout << "<SOLN>" << std::endl;
	for(int i = 0; i <= ati.num_times; i++) {
		if(ati.g[ati.timenodes[i]].closed) continue;
		std::cout << "t = " << ati.g[ati.timenodes[i]].t << std::endl;
	}
	std::cout << "</SOLN>" << std::endl;
}

void print_all_timeslots(ATI &ati) {
	graph_traits<Graph>::edge_iterator ei, ei_end;
	std::cout << "<GRAPH>" << std::endl;
	for(tie(ei, ei_end) = edges(ati.g); ei != ei_end; ++ei) {
		Vdesc v1 = source(*ei, ati.g), v2 = target(*ei, ati.g);
		int cap = get(edge_capacity, ati.g, *ei);

		std::cout << "(";
		if(ati.g[v1].type == JOB) { std::cout << "j" << ati.g[v1].jobid; }
		else if(ati.g[v1].type == TIME) { std::cout << "t" << ati.g[v1].t; }
		else if(ati.g[v1].type == SRC) { std:: cout << "src"; }
		else if(ati.g[v1].type == SINK) { std:: cout << "sink"; }
		else std::cout << "invalid type";
		std::cout << " --> ";

		if(ati.g[v2].type == JOB) { std::cout << "j" << ati.g[v2].jobid; }
		else if(ati.g[v2].type == TIME) { std::cout << "t" << ati.g[v2].t; }
		else if(ati.g[v2].type == SRC) { std:: cout << "src"; }
		else if(ati.g[v2].type == SINK) { std:: cout << "sink"; }
		else std::cout << "invalid type";
		std:: cout << ")";
		
		std::cout << "  " << cap << std:: endl;
	}
	std::cout << "</GRAPH>" << std::endl;
}


// add job vertex to ATI with release time r, deadline d and processing time p
void add_job(ATI &ati, int r, int d, int p) {
	assert(ati.num_jobs < MAXJOBS);
	int i = ati.num_jobs;
	ati.jobnodes[i] = add_vertex(ati.g);
	ati.g[ati.jobnodes[i]].jobid = i;
	ati.g[ati.jobnodes[i]].r = r;
	ati.g[ati.jobnodes[i]].d = d;
	ati.g[ati.jobnodes[i]].p = p;
	ati.g[ati.jobnodes[i]].type = JOB;
	ati.num_jobs++;

	if(d > ati.num_times) ati.num_times = d;

	ati.processing_time_sum += p;
}

// add all time slots in range [0, t]
void add_time_vertices(ATI &ati, int t) {
	for(int i = 0; i <= t; i++) {
		ati.timenodes[i] = add_vertex(ati.g);
		ati.g[ati.timenodes[i]].t = i;
		ati.g[ati.timenodes[i]].closed = false;
		ati.g[ati.timenodes[i]].type = TIME;
	}
}

void ati_init(ATI &ati) {
	ati.rev = get(edge_reverse, ati.g);
	ati.capacity_map = get(edge_capacity, ati.g);

	ati.src = add_vertex(ati.g);
	ati.g[ati.src].type = SRC;
	ati.sink = add_vertex(ati.g);
	ati.g[ati.sink].type = SINK;

	memset(ati.jobnodes, 0, MAXJOBS * sizeof(int));
	memset(ati.timenodes, 0, MAXTIMES * sizeof(int));
	ati.num_jobs = 0;
	ati.num_times = 0;
}

void ati_jobadd_complete(ATI &ati) {
	add_time_vertices(ati, ati.num_times);
	construct_edges(ati);
}

void construct_edges(ATI &ati) {
	// add time nodes and parallel parameter (B or g) capacity edges to sink
	for(int t = 0; t <= ati.num_times; t++) {
		AddEdge(ati, ati.timenodes[t], ati.sink, ati.cap);
	}

	// add job related edges
	for(int i = 0; i < ati.num_jobs; i++) {
		// add src to job edges with capacity = processing time of job
		AddEdge(ati, ati.src, ati.jobnodes[i], ati.g[ati.jobnodes[i]].p);
		// add unit capacity edges between job nodes and time nodes
		for(int t = ati.g[ati.jobnodes[i]].r; t <= ati.g[ati.jobnodes[i]].d; t++) {
			AddEdge(ati, ati.jobnodes[i], ati.timenodes[t], 1);
		}
	}
}

bool feasible_schedule_exists(ATI &ati) {
	return ati.processing_time_sum <= push_relabel_max_flow(ati.g, ati.src, ati.sink);
}


void close_timeslot(ATI &ati, Vdesc t) {
	std::pair<Edesc, bool> todelete, todelete_rev;

	todelete = edge(t, ati.sink, ati.g);
	todelete_rev = edge(ati.sink, t, ati.g);
	put(edge_capacity, ati.g, todelete.first, 0);
	put(edge_capacity, ati.g, todelete_rev.first, 0);
}

void open_timeslot(ATI &ati, Vdesc t) {
	std::pair<Edesc, bool> torestore, torestore_rev;

	torestore = edge(t, ati.sink, ati.g);
	torestore_rev = edge(ati.sink, t, ati.g);
	put(edge_capacity, ati.g, torestore.first, ati.cap);
	put(edge_capacity, ati.g, torestore_rev.first, 0);
}
