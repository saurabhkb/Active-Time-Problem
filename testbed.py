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
import random

MAX_FAILURES = 100
PROB = 0.5

def random_unit(G, T, job_start, processing_time_sum):
	r = random.randint(0, T)	# randint returns a random number in the closed interval [0, T]
	d = random.randint(r, T)
	p = random.randint(1, d - r + 1)	# range is [r, d] => d - r + 1 slots

	G.add_edge('src', "j%d" % job_start, capacity=p)
	for t in range(r, d + 1):
		G.add_edge("j%d" % job_start, "t%d" % t, capacity=1)

	if nx.maximum_flow(G, 'src', 'sink')[0] < processing_time_sum + p:
		# no it does not, so remove any evidence of its existence
		G.remove_edge('src', "j%d" % job_start)
		for t in range(r, d + 1):
			G.remove_edge("j%d" % job_start, "t%d" % t)
		return {'success': False, 'new_job_idx': -1, 'new_job_mass': -1, 'new_jobs': []}
	else:
		return {'success': True, 'new_job_idx': job_start + 1, 'new_job_mass': p, 'new_jobs': [(r, d, p)]}


def adversarial_unit(G, pos, job_start, x, g, processing_time_sum):
	'''
	generic adversarial unit
	x slots containing x * g unit jobs live in [pos, pos + x + g]
	g slots containing g - x rigid jobs live in [pos + x, pos + x + g)
	x jobs of length g live in [pos + x, pos + x + 2g]

	ideally, algorithm should open x slots initially and schedule the unit jobs in them
	then, the g length jobs could be scheduled on top of the rigid ones giving a total cost of x + g
	however, if any y of the x slots are closed by our algorithm, that will push at least y * g unit jobs
	over the rigid region and in the worst case occupy y of the x free rows above the rigid jobs and push
	the g length jobs out. This would give a total cost of x - y + 2g. In the best case, the y * g job mass would
	occupy a block of width (y * g) / x and the long jobs could overlap the rigid jobs by amount (g - y * g / x) thereby giving a total cost of x - y + g + y * g / x

	this method will attempt to add such a unit at position pos in the graph by adding its own jobs numbered
	starting from job_start
	'''
	# construct jobs
	unit_jobs = range(job_start, job_start + x * g)
	job_start += x * g
	rigid_jobs = range(job_start, job_start + (g - x))
	job_start += g - x
	long_jobs = range(job_start, job_start + x)
	job_start += x

	# add them in to the graph
	for j in unit_jobs: G.add_edge('src', 'j%d' % j, capacity=1)
	for j in rigid_jobs: G.add_edge('src', 'j%d' % j, capacity=g)
	for j in long_jobs: G.add_edge('src', 'j%d' % j, capacity=g)

	# add edges to valid time slots
	for j in unit_jobs:
		for t in range(pos, pos + x + g):
			G.add_edge('j%d' % j, 't%d' % t, capacity=1)
	for j in rigid_jobs:
		for t in range(pos + x, pos + x + g):
			G.add_edge('j%d' % j, 't%d' % t, capacity=1)
	for j in long_jobs:
		for t in range(pos + x, pos + x + 2 * g):
			G.add_edge('j%d' % j, 't%d' % t, capacity=1)

	# print "unit=", range(pos, pos + x + g)
	# print "rigid=", range(pos + x, pos + x + g)
	# print "long=", range(pos + x, pos + x + 2 * g)
	if nx.maximum_flow(G, 'src', 'sink')[0] < processing_time_sum + 1 * g * x + g * (g - x) + g * x:
		# infeasible => destroy evidence
		for j in unit_jobs: G.remove_edge('src', 'j%d' % j)
		for j in rigid_jobs: G.remove_edge('src', 'j%d' % j)
		for j in long_jobs: G.remove_edge('src', 'j%d' % j)
		for j in unit_jobs:
			for t in range(pos, pos + x + g):
				G.remove_edge('j%d' % j, 't%d' % t)
		for j in rigid_jobs:
			for t in range(pos + x, pos + x + g):
				G.remove_edge('j%d' % j, 't%d' % t)
		for j in long_jobs:
			for t in range(pos + x, pos + x + 2 * g):
				G.remove_edge('j%d' % j, 't%d' % t)
		return {'success': False, 'new_job_idx': -1, 'new_job_mass': -1, 'new_jobs': []}
	else:
		return {'success': True, 'new_job_idx': job_start, 'new_job_mass': 1 * g * x + g * (g - x) + g * x, 'new_jobs': [(pos, pos + x + g, 1)] * (g * x) + [(pos + x, pos + x + g - 1, g)] * (g - x) + [(pos + x, pos + x + 2 * g, g)] * x}


def generate_instance(n, T, g):
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
	num_failures = 0
	num_random = 0
	num_adv = 0
	while i < n and num_failures < 100:
		# with probability PROB generate an adversarial unit
		if random.random() < PROB:
			x = random.randint(1, 3)
			# x = 1
			# print T, g, x, T - 2 * g - x
			if T - 2 * g - x < 0:
				result = {'success': False}
			else:
				result = adversarial_unit(G, random.randint(0, T - 2 * g - x), i, x, g, processing_time_sum)
				num_adv += 1
			# result = adversarial_unit(G, 1, i, x, g, processing_time_sum)
		else:
			result = random_unit(G, T, i, processing_time_sum)
			num_random += 1

		# print result
		if not result['success']:
			num_failures += 1
		else:
			num_failures = 0
			i = result['new_job_idx']
			processing_time_sum += result['new_job_mass']
			jobs += result['new_jobs']
	print num_adv, num_random
	return jobs

while True:
	# read parameters in from stdin
	try:
		l = sys.stdin.readline()
		if l == "" or l == None: break
		if l.startswith("#"): continue
		num_instances, N1, N2, M1, M2, B = map(int, l.strip().split())
	except EOFError as e:
		break

	# create the directory unless one already exists
	dirname = "data/adv%d_%d_%d_%d_%d" % (N1, N2, M1, M2, B)
	if not os.path.exists(dirname):
		os.makedirs(dirname)
	print "dir=", dirname

	# cycle through the instance list and generate instances accordingly
	for inst in range(num_instances):
		# generate random parameters using ranges given for the instance
		print inst
		n = random.randint(N1, N2)
		T = random.randint(M1, M2)
		g = random.randint(B / 2, B)

		# print n, T, g
		jobs = generate_instance(n, T, g)
		# for j in jobs: print j

		# write that schedule to a file
		with open("%s/inst%d" % (dirname, inst), "w") as f:
			f.write("%d\n" % g)
			for j in jobs:
				f.write("%d %d %d\n" % (j[0], j[1], j[2]))

