#include <stdio.h>
#include <unistd.h>
#include <libworld/world.h>

static int myRobot = 0;

void worldStatusCallback(const WorldStatus* ws, void* additional);
void worldStatusCallback(const WorldStatus* ws, void* additional) {
	for(size_t i = 0; i < ws->numOfObjects; i++) {
		printf("object %d is a %s\n", ws->objects[i].id, ws->objects[i].type == ROBOT ? "robot" : "fuel station");

		if(ws->objects[i].type == ROBOT && myRobot == ws->objects[i].id) {
			MoveRobot(additional, ws->objects[i].id, 1, 1);
		}
	}
}

int main(int argc, char** argv) {
	void* worldCtx = NULL;

	if(argc < 2) {
		printf("%s host\n", argv[0]);
		return 0;
	}

	if(!(worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	myRobot = createRobot(worldCtx);
	startProcessingWorldEvents(worldCtx, &worldStatusCallback, worldCtx);

	while(1) {
		sleep(1);
	}

	detachFromWorld(worldCtx);
}
