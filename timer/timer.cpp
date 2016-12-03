#include "timer.h"

void start_timer(Timer &t) {
	clock_gettime(CLOCK_REALTIME, &(t.start));
}

void stop_timer(Timer &t) {
	clock_gettime(CLOCK_REALTIME, &(t.end));
}

long get_duration(Timer &t) {
	struct timespec start = t.start, stop = t.end;
	struct timespec diff;
	if ((stop.tv_nsec - start.tv_nsec) < 0) {
		diff.tv_sec = stop.tv_sec - start.tv_sec - 1;
		diff.tv_nsec = stop.tv_nsec - start.tv_nsec + 1000000000;
	} else {
		diff.tv_sec = stop.tv_sec - start.tv_sec;
		diff.tv_nsec = stop.tv_nsec - start.tv_nsec;
	}
	return 1e9 * diff.tv_sec + diff.tv_nsec;
}
