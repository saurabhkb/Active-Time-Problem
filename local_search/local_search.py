import networkx as nx
import itertools
from PIL import Image, ImageDraw, ImageFont
from argparse import ArgumentParser

class Schedule:
	def __init__(self):
		self.jobs = []
		self.SOURCE = "SRC"
		self.SINK = "SINK"

	def read_schedule_file(self, fname):
		''' Expects a file with lines of the form "time:job1,job2,...".
		This function reads this file and constructs the schedule in memory.
		Then it looks at the schedule and sets to zero the capacities of timeslot-SINK edges
		that are closed in the schedule. '''

		schedule = [None for t in range(self.T)]
		with open(fname) as f:
			for line in f:
				if len(line.strip()) == 0: continue
				if line.strip().startswith("#"): continue
				tok = line.strip().split(":")
				slot_number = int(tok[0])
				slot_jobs = tok[1].split(",")
				schedule[slot_number] = slot_jobs

		for t, tval in enumerate(schedule):
			if tval == None:
				# t is closed so set the corresponding time node - SINK edge to zero capacity
				self.g.edge[t][self.SINK]['capacity'] = 0
		if not self.is_schedule_feasible():
			return False
		return True

	def read_job_data(self, fname, B):
		''' expects a file with lines of the form "job_id release_time deadline processing_length".
		This function reads this file and constructs a bipartite graph of jobs and timeslots for
		future flow computations. '''

		jobs = {}
		with open(fname) as f:
			for line in f:
				if len(line.strip()) == 0: continue
				if line.strip().startswith("#"): continue
				job_id, r, d, p = line.strip().split()
				jobs[job_id] = map(int, (r, d, p))

		self.T = max([jobs[j][1] for j in jobs]) + 1	# T = maximum deadline + 1. range = [0, T)
		self.jobs = jobs.keys()
		self.B = B

		self.g = nx.DiGraph()

		# add source and sink nodes
		self.g.add_nodes_from([self.SOURCE, self.SINK])

		# add timeslot nodes and join them to the sink
		for t in range(self.T):
			self.g.add_edge(t, self.SINK, capacity=self.B)

		for j in jobs:
			# edge from source to job node with capacity = p_j
			self.g.add_edge(self.SOURCE, j, capacity=jobs[j][2])

			# edge from job node to each valid time node [r, d]
			for t in range(jobs[j][0], jobs[j][1] + 1):
				self.g.add_edge(j, t, capacity=1)


	def is_schedule_feasible(self):
		''' checks if graph in its current state is a feasible schedule. '''
		tot_processing_time = sum(self.g.edge[self.SOURCE][e]['capacity'] for e in self.g.edge[self.SOURCE])
		return tot_processing_time == nx.maximum_flow_value(self.g, self.SOURCE, self.SINK)


	def find_minimal_feasible(self, blacklist=[]):
		''' Tries to close down as many slots as possible until no longer possible.
		But don't close the slots in blacklist. '''
		slots_closed = []
		for t in range(self.T):
			if t in blacklist: continue
			if self.g[t][self.SINK]['capacity'] == 0: continue
			self.g[t][self.SINK]['capacity'] = 0
			if self.is_schedule_feasible():
				slots_closed.append(t)
			else:
				self.g[t][self.SINK]['capacity'] = self.B
		return slots_closed

	def local_b(self, b):
		''' applies the "open b, close > b" rule repeatedly until local optimum is reached. '''

		schedule_changed = True
		while schedule_changed:
			# find set of closed slots
			empty = []
			for t in range(self.T):
				if self.g.edge[t][self.SINK]['capacity'] == 0:
					empty.append(t)

			if len(empty) < b:
				break

			schedule_changed = False
			for slots_to_open in itertools.combinations(empty, b):
				gcpy = self.g.copy()

				# open the slots
				for t in slots_to_open:
					self.g.edge[t][self.SINK]['capacity'] = self.B

				# attempt to close as many slots as possible
				closed_slots = self.find_minimal_feasible(slots_to_open)

				if len(closed_slots) <= b:
					self.g = gcpy	# if number closed <= b, then this opening wasn't worth it
				else:
					schedule_changed = True
					print "opened slots %s and closed %s" % (",".join(map(str, slots_to_open)), ",".join(map(str, closed_slots)))
					break	# otherwise break so that we can recompute the empty slots


	def get_schedule(self):
		''' returns the current stored schedule as a list of lists of jobs. '''
		f, flow_dict = nx.maximum_flow(self.g, self.SOURCE, self.SINK)
		schedule = [[] for t in range(self.T)]
		for j in self.jobs:
			j_flow = flow_dict[j]
			for t in j_flow:
				if j_flow[t] == 1:
					schedule[t].append(j)
		return schedule

	def print_schedule(self):
		''' helper method which prints out schedule in a nice way. '''
		s = self.get_schedule()
		num_open = len([1 for x in s if len(x) > 0])
		print num_open
		for t, tjobs in enumerate(s):
			if tjobs != []:
				print t, ":", ",".join(map(str, tjobs))

	def draw_schedule(self, output="hello.png"):
		schedule = self.get_schedule()
		fontsize = 15
		img = Image.new('RGB', (len(schedule) * 50, self.B * 50), "white")
		draw = ImageDraw.Draw(img)
		fnt = ImageFont.truetype("arial.ttf", fontsize)
		for x, jobs in enumerate(schedule):
			for y, j in enumerate(jobs):
				draw.text((x * 40, (self.B - y) * 30), str(j), (3, 3, 3), fnt)
		img.save(output)


def main():
	parser = ArgumentParser(description="Run the local search algorithm on the given schedule.")
	parser.add_argument('B', type=int, metavar="B", help="parallelism parameter")
	parser.add_argument('jf', type=str, metavar="job_file", help="path to file with job data")
	parser.add_argument('sf', type=str, metavar="schedule_file", help="path to file with schedule")
	parser.add_argument('ls_param', type=int, default=2, metavar="ls_param", help="local search parameter")
	ns = parser.parse_args()

	S = Schedule()
	S.read_job_data(ns.jf, ns.B)
	if not S.read_schedule_file(ns.sf):
		print "Schedule not feasible!"
	else:
		S.draw_schedule("before.png")
		for i in range(1, ns.ls_param + 1):
			S.local_b(i)
		S.print_schedule()
		S.draw_schedule("after.png")

if __name__ == "__main__":
	main()
