#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "../timer/timer.h"
#include <iostream>

#define MAXJOBS	100
#define MAXTIMES	200

//using namespace boost;

// type of the node in the graph
enum Type {JOB, TIME, SRC, SINK};

// definitions taken from:
// http://programmingexamples.net/wiki/CPP/Boost/BGL/MaxFlow
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
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

typedef boost::adjacency_list<
	boost::listS,
	boost::vecS,
	boost::directedS,
	Vertex,	// vertex properties
	boost::property<boost::edge_capacity_t, int,			// edge capacity
		boost::property<boost::edge_residual_capacity_t, int,		// edge residual capacity
			boost::property<boost::edge_reverse_t, Edesc> > >		// reverse edge descriptor
> Graph;

class Schedule {
public:
	int processing_time_sum;
	int num_jobs;
	int num_times;
	int cap;

	Schedule();
	void minimal_feasible_schedule();
	int get_num_open_timeslots();
	bool is_closed(int);
	void read_job_data();
	bool is_feasible();
	void close_timeslot(int);
	void open_timeslot(int);

private:
	Graph g;
	boost::property_map <Graph, boost::edge_reverse_t>::type rev;
	boost::property_map <Graph, boost::edge_capacity_t>::type capacity_map;
	Vdesc src, sink;
	Vdesc jobnodes[MAXJOBS], timenodes[MAXTIMES];

	Edesc add_edge_wrapper(Vdesc &, Vdesc &, double);
	void add_job(int, int, int);
};


