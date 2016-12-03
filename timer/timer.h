#include <ctime>

typedef struct {
	struct timespec start, end;
} Timer;

void start_timer(Timer &);
void stop_timer(Timer &);
long get_duration(Timer &);
