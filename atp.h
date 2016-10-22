#include <boost/config.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <cstring>
#include <cassert>

#define JOB		0
#define TIME	1
#define SRC		2
#define SINK	3

#define MAXJOBS	100
#define MAXTIMES	200

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

typedef struct {
	Graph g;
	property_map <Graph, edge_reverse_t>::type rev;
	property_map <Graph, edge_capacity_t>::type capacity_map;
	Vdesc src, sink;
	Vdesc jobnodes[MAXJOBS], timenodes[MAXTIMES];
	int processing_time_sum;
	int num_jobs;
	int num_times;
	int cap;
} ATI;


Edesc AddEdge(ATI &, Vdesc &, Vdesc &, double);
void minimal_feasible_schedule(ATI &);
void print_open_timeslots(ATI &);
void print_all_timeslots(ATI &);
void add_job(ATI &, int, int, int);
void ati_jobadd_complete(ATI &);
void ati_init(ATI &, int);
bool feasible_schedule_exists(ATI &);
void close_timeslot(ATI &, Vdesc);
void open_timeslot(ATI &, Vdesc);

