'''
Adapted from the empirical evaluation section of Jessica Chang's PhD thesis

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
from random import randint

def random_job(T):
	r = randint(0, T)
	d = randint(r, T)
	p = randint(1, d - r + 1)	# range is [r, d] => d - r + 1 slots
	return (r, d, p)

while True:
	# read parameters in from stdin
	try:
		l = sys.stdin.readline()
		if l == "" or l == None: break
		num_instances, N1, N2, M1, M2, B = map(int, l.strip().split())
	except EOFError as e:
		break

	# create the directory unless one already exists
	dirname = "data/p%d_%d_%d_%d_%d" % (N1, N2, M1, M2, B)
	if not os.path.exists(dirname):
		os.makedirs(dirname)

	# cycle through the instance list and generate instances accordingly
	for inst in range(num_instances):
		# generate random parameters using ranges given for the instance
		n = randint(N1, N2)
		T = randint(M1, M2)
		g = randint(B / 2, B)

		# create a bipartite digraph representing the instance
		G = nx.DiGraph()
		G.add_node('src')
		G.add_node('sink')

		# keep randomly generating jobs until a feasible schedule is obtained
		jobs = []
		for t in range(T):
			G.add_edge("t%d" % t, 'sink', capacity=g)
		processing_time_sum = 0
		jobs = []
		i = 0
		while i < n:
			r, d, p = random_job(T)

			# tentatively add job in to the graph
			processing_time_sum += p
			G.add_edge('src', "j%d" % i, capacity=p)
			for t in range(r, d + 1):
				G.add_edge("j%d" % i, "t%d" % t, capacity=1)

			# check if job maintains feasibility of instance
			if nx.maximum_flow(G, 'src', 'sink')[0] < processing_time_sum:
				# no it does not, so remove any evidence of its existence
				G.remove_edge('src', "j%d" % i)
				for t in range(r, d + 1):
					G.remove_edge("j%d" % i, "t%d" % t)
				processing_time_sum -= p
			else:
				# yes it does, so add it in and keep the updates made
				jobs.append((r, d, p))
				i += 1

		# shift schedule so that at least one job starts at time 0
		delta = min([j[0] for j in jobs])	# earliest release time

		# write that schedule to a file
		with open("%s/inst%d" % (dirname, inst), "w") as f:
			f.write("%d\n" % g)
			for j in jobs:
				f.write("%d %d %d\n" % (j[0] - delta, j[1] - delta, j[2]))

