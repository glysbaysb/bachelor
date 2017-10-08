#include <stdio.h>
#include <unistd.h>
#include <libworld/world.h>

typedef struct {
	int robot;
	void* worldCtx;
} Info;

void worldStatusCallback(const WorldStatus* ws, void* additional);
void worldStatusCallback(const WorldStatus* ws, void* additional)
{
	Info* info = (Info*)additional;

	printf("(%f:%f)\n", ws->xTilt, ws->yTilt);

	for(size_t i = 0; i < ws->numOfObjects; i++) {
		printf("#%d is a %s\n", ws->objects[i].id, ws->objects[i].type == ROBOT ? "robot" : "fuel station");
		printf("\tPos: (%f:%f)\n", ws->objects[i].x, ws->objects[i].y);
		printf("\tMass: %f\n", ws->objects[i].m);

		if(ws->objects[i].type == ROBOT && info->robot == ws->objects[i].id) {
			printf("\tFuel: %d\n", ws->objects[i].fuel);

			int r = 0;
			if((r = MoveRobot(info->worldCtx, ws->objects[i].id, 1, 1)) < 0) {
				fprintf(stderr, "can't move robot: %d", r);
			}
		}
	}
}

int main(int argc, char** argv)
{
	Info callbackInfo = {0};

	if(argc < 2) {
		printf("%s host\n", argv[0]);
		return 0;
	}

	if(!(callbackInfo.worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	callbackInfo.robot = createRobot(callbackInfo.worldCtx);
	printf("Created robot %d\n", callbackInfo.robot);
	startProcessingWorldEvents(callbackInfo.worldCtx, &worldStatusCallback, (void*)&callbackInfo);

	while(1) {
		sleep(1);
	}

	detachFromWorld(callbackInfo.worldCtx);
}
