#include <boost/config.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>

#define JOB		0
#define TIME	1
#define SRC		2
#define SINK	3

using namespace boost;

// definitions taken from:
// http://programmingexamples.net/wiki/CPP/Boost/BGL/MaxFlow
typedef adjacency_list_traits<vecS, vecS, directedS> Traits;
typedef Traits::vertex_descriptor Vdesc;
typedef Traits::edge_descriptor Edesc;

typedef struct {
	// to be used by job nodes
	int jobid;
	int r, d, p;

	// to be used by time nodes
	int t;
	bool closed;
	int type;
} Vertex;

typedef adjacency_list<
	listS,
	vecS,
	directedS,
	Vertex,	// vertex properties
	property<edge_capacity_t, int,			// edge capacity
		property<edge_residual_capacity_t, int,		// edge residual capacity
			property<edge_reverse_t, Edesc> > >		// reverse edge descriptor
> Graph;

// helper method to create edge between v1 and v2
Edesc AddEdge(Vdesc &v1, Vdesc &v2, property_map <Graph, edge_reverse_t>::type &rev, double capacity, Graph &g) {
	Edesc e1 = add_edge(v1, v2, g).first;
	Edesc e2 = add_edge(v2, v1, g).first;
	put(edge_capacity, g, e1, capacity);
	put(edge_capacity, g, e2, 0);

	rev[e1] = e2;
	rev[e2] = e1;
}

#define MAX	10
#define MAXCAP 5

void minimal_feasible_schedule(Graph &g, Vdesc jobs[], Vdesc times[], int num_times, Vdesc src, Vdesc &sink, int processing_time_sum, property_map <Graph, edge_reverse_t>::type &rev) {
	// iterate through time slots shutting them down and repeat till no longer possible
	int temp, cap;
	std::pair<Edesc, bool> p, p_rev;
	Edesc e, e_rev;
	bool closed = false;
	while(1) {
		closed = false;
		for(int i = 0; i < num_times; i++) {
			if(g[times[i]].closed) continue;
			remove_edge(times[i], sink, g);
			AddEdge(times[i], sink, rev, 0, g);
			// remove_edge(times[i], sink, g);
			// remove_edge(sink, times[i], g);
			if(push_relabel_max_flow(g, src, sink) < processing_time_sum) {
				// infeasible so add the edges back in
				remove_edge(times[i], sink, g);
				AddEdge(times[i], sink, rev, MAXCAP, g);
			} else {
				g[times[i]].closed = true;
				closed = true;
			}
			
		}
		if(!closed) break;
	}
}

void print_open_timeslots(Graph &g, Vdesc times[], int num_times) {
	Vdesc t;
	std::cout << "<SOLN>" << std::endl;
	for(int i = 0; i <= num_times; i++) {
		if(g[times[i]].closed) continue;
		std::cout << "t = " << g[times[i]].t << std::endl;
	}
	std::cout << "</SOLN>" << std::endl;
}

void print_all_edges(Graph &g) {
	graph_traits<Graph>::edge_iterator ei, ei_end;
	std::cout << "<GRAPH>" << std::endl;
	for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
		Vdesc v1 = source(*ei, g), v2 = target(*ei, g);
		int cap = get(edge_capacity, g, *ei);

		std::cout << "(";
		if(g[v1].type == JOB) { std::cout << "j" << g[v1].jobid; }
		else if(g[v1].type == TIME) { std::cout << "t" << g[v1].t; }
		else if(g[v1].type == SRC) { std:: cout << "src"; }
		else if(g[v1].type == SINK) { std:: cout << "sink"; }
		else std::cout << "invalid type";
		std::cout << " --> ";

		if(g[v2].type == JOB) { std::cout << "j" << g[v2].jobid; }
		else if(g[v2].type == TIME) { std::cout << "t" << g[v2].t; }
		else if(g[v2].type == SRC) { std:: cout << "src"; }
		else if(g[v2].type == SINK) { std:: cout << "sink"; }
		else std::cout << "invalid type";
		std:: cout << ")";
		
		std::cout << "  " << cap << std:: endl;
	}
	std::cout << "</GRAPH>" << std::endl;
}

int main() {
	int num_jobs = 0;
	int time_lb = 100, time_ub = -1;
	int processing_time_sum = 0;

	/* === build bipartite graph === */

	Graph g;
	property_map<Graph, edge_reverse_t>::type rev = get(edge_reverse, g);
	property_map < Graph, edge_capacity_t >::type capacity_map = get(edge_capacity, g);

	Vdesc src = add_vertex(g);
	g[src].type = SRC;
	Vdesc sink = add_vertex(g);
	g[sink].type = SINK;

	Vdesc jobnodes[MAX], timenodes[10 * MAX];

	// read in jobs data and add job nodes
	int r, d, p;
	while(std::cin >> r >> d >> p) {
		jobnodes[num_jobs] = add_vertex(g);
		g[jobnodes[num_jobs]].jobid = num_jobs;
		g[jobnodes[num_jobs]].r = r;
		g[jobnodes[num_jobs]].d = d;
		g[jobnodes[num_jobs]].p = p;
		g[jobnodes[num_jobs]].type = JOB;

		// update lower and upper bounds on time
		if(r < time_lb) time_lb = r;
		if(d > time_ub) time_ub = d;

		processing_time_sum += p;
		num_jobs++;
	}

	// add time nodes and parallel parameter (B or g) capacity edges to sink
	for(int t = time_lb; t <= time_ub; t++) {
		timenodes[t - time_lb] = add_vertex(g);
		g[timenodes[t - time_lb]].t = t;
		g[timenodes[t - time_lb]].closed = false;
		g[timenodes[t - time_lb]].type = TIME;
		AddEdge(timenodes[t - time_lb], sink, rev, MAXCAP, g);
	}

	// add job related edges
	for(int i = 0; i < num_jobs; i++) {
		// add src to job edges with capacity = processing time of job
		AddEdge(src, jobnodes[i], rev, g[jobnodes[i]].p, g);
		// add unit capacity edges between job nodes and time nodes
		for(int t = g[jobnodes[i]].r; t <= g[jobnodes[i]].d; t++) {
			AddEdge(jobnodes[i], timenodes[t - time_lb], rev, 1, g);
		}
	}

	int flow = push_relabel_max_flow(g, src, sink);
	std::cout << "flow = " << flow << std::endl;

	if(processing_time_sum > flow) {
		// no feasible schedule exists
		std::cout << "No feasible schedule!" << std::endl;
		return 0;
	}

	// obtain minimal feasible schedule
	// minimal_feasible_schedule(g, jobnodes, timenodes, time_ub - time_lb, src, sink, processing_time_sum, rev);
	// print_open_timeslots(g, timenodes, time_ub - time_lb);

	print_open_timeslots(g, timenodes, time_ub - time_lb);

	int temp, cap, num_times = time_ub - time_lb;
	std::pair<Edesc, bool> todelete, todelete_rev;
	bool closed = false;
	while(1) {
		closed = false;
		for(int i = 0; i <= num_times; i++) {
			if(g[timenodes[i]].closed)
				continue;
			todelete = edge(timenodes[i], sink, g);
			todelete_rev = edge(sink, timenodes[i], g);
			temp = get(edge_capacity, g, todelete.first);
			put(edge_capacity, g, todelete.first, 0);
			put(edge_capacity, g, todelete_rev.first, 0);
			if(push_relabel_max_flow(g, src, sink) < processing_time_sum) {
				// infeasible so add the edges back in
				put(edge_capacity, g, todelete.first, temp);
				put(edge_capacity, g, todelete_rev.first, 0);
			} else {
				g[timenodes[i]].closed = true;
				closed = true;
			}
			
		}
		if(!closed) break;
		std::cout << "closed one so loop again" << std::endl;
	}

	print_open_timeslots(g, timenodes, time_ub - time_lb);

	return 0;
}

