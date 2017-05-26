/**
 * @file
 * @section DESCRIPTION
 * This is the simulation for the XXXX.
 * 
 * 
 */
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <sys/timerfd.h>
#include <poll.h>
#include <unistd.h>
#include <assert.h>
#include <memory>
#include <utility>
#include <vector>
#include <cmath>

#include "world.h"

/**
 * @brief creates a timerfd
 *
 * Creates a timerfd, with the specified inital expiration and interval values.
 * @param secs the second part of the inital expiration and interval value 
 * @param secs the nanosecond partof the inital expiration and interval value
 * @return a valid fd or something < 0
 */
static int create_timerfd(int secs, int nsecs) {
	int tfd;
	if ((tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK)) == 1) {
        perror("timerfd_create");
        return tfd;
    }
    struct itimerspec timer = {secs, nsecs, secs, nsecs};
    if (timerfd_settime(tfd, 0, &timer, NULL) == -1) {
        perror("timerfd_set");
        return tfd;
    }
	
	return tfd;
}

/**
 * Returns a (static) string that holds the current time
 */
static const char* get_time() {
	static char buffer[32] = {0};

	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);

	time_t s = spec.tv_sec;
	long ms = lround(spec.tv_nsec / 1.0e6); // todo: man math_error or c++11 std::lround

	int ret = snprintf(buffer, sizeof(buffer), "[%" PRIdMAX ".%03ld]", (intmax_t)s, ms);
	if(ret > sizeof(buffer) || ret < 0) {
		perror("get_time(): snprintf");
		exit(-1);
	}
	
	return buffer;
}

int main(int argc, char** argv) {
	/* arg parsing */
	if(argc < 2) {
		printf("%s numberofrobots\n", argv[0]);
		return 0;
	}
	
	int numberOfRobots = std::atoi(argv[1]);
	
	/* init */
#ifdef DEBUG
	#define INTERVALL_S 0
	#define INTERVALL_NS 100*1000*1000
#else
	#define INTERVALL_S 1
	#define INTERVALL_NS 0
#endif
	int timer = create_timerfd(INTERVALL_S, INTERVALL_NS);
	if(timer < 0) {
		return 1;
	}
	
	/* simulation loop */
	struct pollfd pfd[] = {
        {.fd = timer, .events = POLLIN, 0},
	};
	int s;
	while((s = poll(pfd, (sizeof(pfd) / sizeof(pfd[0])), -1)) > 0) {
		/* re-arm timer */
		if((pfd[0].revents & POLLIN) == POLLIN) {
			uint64_t expired;

			pfd[0].revents = 0;
			read(pfd[0].fd, &expired, sizeof(uint64_t));
			
			printf("%s next simulation step\n", get_time());
		}
		
		/* get all robot commands */
		
		/* update robot positions */
		
		/* recalc world */
		
		// collision
		// degree of plate
		// ?
		
		/* recalc fuel status */
		
		/* calculate minus points */
		
		/* send out game state - over the unsafe & safe network */
	}
	
	/* cleanup */
	return 0;
}