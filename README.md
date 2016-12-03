# Active-Time-Problem
Boost and IBFS based implementations of the MINFEAS and GREEDY algorithms for the active time problem.

Running `make` within the boost and ibfs directories will create the corresponding minfeas and greedy executables.
All executables expect as input a file which contains as its first line the value of g (the parallelism parameter) and then a listing of the jobs (one on each line) described by a release time, deadline and processing time.

For eg.
```
6
1 3 2
2 5 1
1 20 10
```

So you can run it as: `./minfeas < problem_instance_file` or `./greedy < problem_instance_file` where the `problem_instance_file` is in the format described above.

The output will be on two lines - the first line will be the time taken to run in ns, the second line will be the cost of the solution.

To run the LP solver in the gurobi directory, you will need to have Gurobi and then it can be run as `gurobi.sh python lpsolve.py < problem_instance_file`.


####  IBFS
The IBFS implementation comes from the Maximum Flow Project (http://www.cs.tau.ac.il/~sagihed/ibfs/code.html) and is based on the following two papers:

"Faster and More Dynamic Maximum Flow 
by Incremental Breadth-First Search"
Andrew V. Goldberg, Sagi Hed, Haim Kaplan, Pushmeet Kohli,
Robert E. Tarjan, and Renato F. Werneck
In Proceedings of the 23rd European conference on Algorithms, ESA'15
2015

"Maximum flows by incremental breadth-first search"
Andrew V. Goldberg, Sagi Hed, Haim Kaplan, Robert E. Tarjan, and Renato F. Werneck.
In Proceedings of the 19th European conference on Algorithms, ESA'11, pages 457-468.
ISBN 978-3-642-23718-8
2011


#### Boost
Boost is available at http://www.boost.org/

#### Gurobi
Gurobi can be obtained from http://www.gurobi.com/.
