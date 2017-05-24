/**
 * This is the simulation for the XXXX.
 * 
 * 
 */
#include <stdio.h>

int main(int argc, char** argv) {
	/* arg parsing */
	if(argc < 2) {
		printf("%s numberofrobots\n", argv[0]);
		return 0;
	}
	
	int numberOfRobots = atoi(argv[1]);
	
	/* init */
	
	/* simulation loop */
	while(1) {
		/* re-arm timer */
		
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