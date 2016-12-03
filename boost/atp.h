#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "../timer/timer.h"
#include <iostream>

#define MAXJOBS	100
#define MAXTIMES	200

using namespace boost;

// type of the node in the graph
enum Type {JOB, TIME, SRC, SINK};

// definitions taken from:
// http://programmingexamples.net/wiki/CPP/Boost/BGL/MaxFlow
typedef adjacency_list_traits<vecS, vecS, directedS> Traits;
typedef Traits::vertex_descriptor Vdesc;
typedef Traits::edge_descriptor Edesc;

typedef struct {
	// determines what type of node this is
	Type type;

	// to be used by job nodes
	int jobid;
	int r, d, p;

	// to be used by time nodes
	int t;
	bool closed;
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
int get_num_open_timeslots(ATI &);
void print_open_timeslots(ATI &);
void print_all_timeslots(ATI &);
void add_job(ATI &, int, int, int);
void ati_jobadd_complete(ATI &);
void ati_init(ATI &);
bool feasible_schedule_exists(ATI &);
void close_timeslot(ATI &, Vdesc);
void open_timeslot(ATI &, Vdesc);
void details(ATI &);
