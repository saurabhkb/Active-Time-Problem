from gurobipy import *
import sys

"""
The LP being solved by the Gurobi solver:

minimize: sum_t(y[t])
subject to:
	x[t, j] <= y[t]					for all j in J, t in [rj, dj]

	sum_j(x[t, j]) <= g * y_t		for all t in T

	sum_t(x[t, j]) >= pj			for all j in J

	x[t, j], y[t] >= 0				for all j in J, t in T
"""

# read the data file
g = int(sys.stdin.readline().strip())
jobs = [map(int, l.split()) for l in sys.stdin]


# obtain parameters
n = len(jobs)
T = max([x[1] for x in jobs])
print "#jobs = ", n, "\n#times = ", T


# create the model and attempt to solve it
try:
	m = Model("LP")
	m.setParam('OutputFlag', False)	# quieten model output
	y = [m.addVar(vtype=GRB.BINARY, name="y_%d" % t) for t in range(T)]
	x = [[m.addVar(vtype=GRB.BINARY, name="x_%d_%d" % (t, j)) for j in range(n)] for t in range(T)]
	m.update()

	m.setObjective(sum(y), GRB.MINIMIZE)

	# constraint 1
	for t in range(T):
		for j in range(n):
			m.addConstr(x[t][j] <= y[t], "c1_%d_%d" % (t, j))

	# constraint 2
	for t in range(T):
		m.addConstr(sum(x[t]) <= g * y[t], "c2_%d" % t)

	# constraint 3
	for j in range(n):
		m.addConstr(sum([x[t][j] for t in range(T)]) >= jobs[j][2], "c3_%d" % j)

	# constraint 4
	for t in range(T):
		m.addConstr(y[t] >= 0, "c5_%d" % t)
		for j in range(n):
			m.addConstr(x[t][j] >= 0, "c4_%d_%d" % (t, j))

	m.optimize()
	print "OPT = ", m.objVal

except GurobiError as e:
	print "Error reported: ", e
