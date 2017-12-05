/**
 * @file A simple test application that tries to move a robot directly. I've
 * used it to check the differential steering algorithm.
 */
#include <iostream>
#include <iterator>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <libworld/world.h>
#include <libnetwork/network.h>
#include <libalgo/algo.h>


typedef struct {
	int robot;
	void* worldCtx;
	Network* network;
} Info;

static void worldStatusCallback(const WorldStatus* ws, void* additional);

int main(int argc, char** argv)
{
	Info callbackInfo = {0};

	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}

	callbackInfo.network = new Network(argv[2]);

	if(!(callbackInfo.worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	callbackInfo.robot = createRobot(callbackInfo.worldCtx);
	printf("Created robot %d\n", callbackInfo.robot);
	startProcessingWorldEvents(callbackInfo.worldCtx, &worldStatusCallback, (void*)&callbackInfo);

	while(1) {
		callbackInfo.network->poll(1);
	}

	detachFromWorld(callbackInfo.worldCtx);
}

static void worldStatusCallback(const WorldStatus* ws, void* additional)
{
	Info* info = (Info*)additional;

	printf("(%f:%f)\n", ws->xTilt, ws->yTilt);

	for(size_t i = 0; i < ws->numOfObjects; i++) {
		printf("#%d is a %s\n", ws->objects[i].id, ws->objects[i].type == ROBOT ? "robot" : "fuel station");
		printf("\tPos: (%f:%f)\n", ws->objects[i].x, ws->objects[i].y);
		printf("\tMass: %f\n", ws->objects[i].m);

		if(ws->objects[i].type == ROBOT) {
			printf("\tFuel: %d\n", ws->objects[i].fuel);

			//moveRobot(info->worldCtx, ws->objects[i].id, 1, -1);
			//moveRobot(info->worldCtx, ws->objects[i].id, 1, 1);
			moveRobot(info->worldCtx, ws->objects[i].id, 1, 0);
		}
	}
}

