#include <stdio.h>
#include <unistd.h>
#include <libworld/world.h>

static int myRobot = 0;

void worldStatusCallback(const WorldStatus* ws, void* additional);
void worldStatusCallback(const WorldStatus* ws, void* additional) {
	printf("(%f:%f)\n", ws->xTilt, ws->yTilt);
	for(size_t i = 0; i < ws->numOfObjects; i++) {
		printf("object %d is a %s\n", ws->objects[i].id, ws->objects[i].type == ROBOT ? "robot" : "fuel station");
		printf("\t(%f:%f)\n",ws->objects[i].x, ws->objects[i].y);

		if(ws->objects[i].type == ROBOT && myRobot == ws->objects[i].id) {
			int r = 0;
			if((r = MoveRobot(additional, ws->objects[i].id, 1, 1)) < 0) {
				fprintf(stderr, "can't move robot: %d", r);
			}
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
	printf("Created robot %d\n", myRobot);
	startProcessingWorldEvents(worldCtx, &worldStatusCallback, worldCtx);

	while(1) {
		sleep(1);
	}

	detachFromWorld(worldCtx);
}
