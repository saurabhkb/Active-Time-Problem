# Active-Time-Problem
A Boost implementation of the MINFEAS and GREEDY algorithms for the active time problem.

Running `make all` will create the minfeas and greedy executables.

Both programs expect as input a file which contains as its first line the value of g (the parallelism parameter) and then
a listing of the jobs (one on each line) described by a release time, deadline and processing time.

For eg.
```
6
1 3 2
2 5 1
1 20 10
```

So you can run it as: `./minfeas < problem_instance_file` or `./greedy < problem_instance_file` where the `problem_instance_file` is in the format described above.
