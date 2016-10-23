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

jobs = []
with open(sys.argv[1]) as f:
	g = int(f.readline().strip())
	for l in f:
		r, d, p = map(int, l.split())
		jobs.append((r, d, p))

# obtain parameters
n = len(jobs)
T = max([x[1] for x in jobs])
print "#jobs = ", n, "\n#times = ", T

# create x[t, j] and y[t] variables
y = [0 for t in range(T)]
x = [[0 for j in range(n)] for t in range(T)]

try:
	m = Model("LP")
	for i in range(T):
		y[i] = m.addVar(vtype=GRB.BINARY, name="y_%d" % i)
	for i in range(T):
		for j in range(n):
			x[i][j] = m.addVar(vtype=GRB.BINARY, name="x_%d_%d" % (i, j))
	m.update()

	m.setObjective(sum(y), GRB.MINIMIZE)

	for t in range(T):
		for j in range(n):
			m.addConstr(x[t][j] <= y[t], "c1_%d_%d" % (t, j))
	for t in range(T):
		m.addConstr(sum(x[t]) <= g * y[t], "c2_%d" % t)
	for j in range(n):
		m.addConstr(sum([x[t][j] for t in range(T)]) >= jobs[j][2], "c3_%d" % j)
	for t in range(T):
		for j in range(n):
			m.addConstr(x[t][j] >= 0, "c4_%d_%d" % (t, j))
	for t in range(T):
		m.addConstr(y[t] >= 0, "c5_%d" % t)

	m.optimize()

	# for v in m.getVars():
	# 	print v.varName, v.x
	print 'Obj:', m.objVal

except GurobiError:
	print "Error reported"

# try:
# 	m = Model("mip1")
# 
# 	# Create variables
# 	x = m.addVar(vtype=GRB.BINARY, name="x")
# 	y = m.addVar(vtype=GRB.BINARY, name="y")
# 	z = m.addVar(vtype=GRB.BINARY, name="z")
# 
# 	# Integrate new variables
# 	m.update()
# 
# 	# Set objective
# 	m.setObjective(x + y + 2 * z, GRB.MAXIMIZE)
# 
# 	# Add constraint: x + 2 y + 3 z <= 4
# 	m.addConstr(x + 2 * y + 3 * z <= 4, "c0")
# 
# 	# Add constraint: x + y >= 1
# 	m.addConstr(x + y >= 1, "c1")
# 
# 	m.optimize()
# 
# 	for v in m.getVars():
# 		print v.varName, v.x
# 	print 'Obj:', m.objVal
# except GurobiError:
# 	print 'Error reported'


