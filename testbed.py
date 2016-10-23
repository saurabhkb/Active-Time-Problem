'''
Adapted from Jessica Chang's PhD thesis

[Chang, Jessica. Energy-aware batch scheduling. Diss. 2013.]


Instance Parameters (N1, N2, M1, M2, B)

N1 <= #jobs <= N2
M1 <= #times <= M2
B / 2 <= capacity <= B

Each job randomly chooses its release time, a deadline for its release time and a job length no more than the length of its feasible window

Expected Input:
x N1 N2 M1 M2 B
.
.
.
This program will create x feasible instances of the active time problem with parameters (N1, N2, M1, M2, B) and store them in a folder named pN1_N2_M1_M2_B and each instance file will be named inst0, inst1, ...
'''

import networkx as nx
import os, sys
import random

def random_job(T):
	r = 0
	d = 0
	p = 0
	while r >= d:
		r = random.randint(0, T)
		if r < T:
			d = random.randint(r + 1, T)
			p = random.randint(1, d - r)
		else: d = -1
	return (r, d, p)

while True:
	try:
		l = sys.stdin.readline()
		if l == "" or l == None: break
		num_instances, N1, N2, M1, M2, B = map(int, l.strip().split())
	except EOFError as e:
		break

	dirname = "data/p%d_%d_%d_%d_%d" % (N1, N2, M1, M2, B)
	if not os.path.exists(dirname):
		os.makedirs(dirname)

	for inst in range(num_instances):
		# generate random parameters using ranges given
		n = random.randint(N1, N2)
		T = random.randint(M1, M2)
		g = random.randint(B / 2, B)

		# create a bipartite digraph representing the instance
		G = nx.DiGraph()
		G.add_node('src')
		G.add_node('sink')

		# keep randomly generating jobs until a feasible schedule is obtained
		infeasible = True
		jobs = []
		while infeasible:
			processing_time_sum = 0
			jobs = []
			for i in range(n):
				r, d, p = random_job(T)
				jobs.append((r, d, p))
				G.add_edge('src', "j%d" % i, capacity=p)
				processing_time_sum += p
				for t in range(r, d + 1):
					G.add_edge("j%d" % i, "t%d" % t, capacity=1)
			for t in range(T):
				G.add_edge("t%d" % t, 'sink', capacity=g)
			infeasible = nx.maximum_flow(G, 'src', 'sink')[0] < processing_time_sum

		# shift schedule so that at least one job starts at time 0
		delta = min([x[0] for x in jobs])	# earliest release time
		# write that schedule to a file
		with open("%s/inst%d" % (dirname, inst), "w") as f:
			for j in jobs:
				f.write("%d %d %d\n" % (j[0] - delta, j[1] - delta, j[2]))

