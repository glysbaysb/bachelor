#include <stdio.h>
#include <unistd.h>
#include "world.h"

void worldStatusCallback(WorldStatus ws, void* additional);
void worldStatusCallback(WorldStatus ws, void* additional) {
	(void) ws; (void) additional;
	printf("worldStatusCallback\n");
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
		MoveRobot(worldCtx, 0, -1, 1);
	}

	detachFromWorld(worldCtx);
}
