#include <stdio.h>
#include <unistd.h>
#include "world.h"

void worldStatusCallback(WorldStatus ws, void* additional) {
	printf("worldStatusCallback\n");

	/* bla bla */
	MoveRobot(additional, 0, -1, 1);
}

int main() {
	void* worldCtx = NULL;
	if(!(worldCtx = connectToWorld())) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	startProcessingWorldEvents(worldCtx, &worldStatusCallback, worldCtx);

	while(1) {
		sleep(1);
	}

	detachFromWorld(worldCtx);
}
