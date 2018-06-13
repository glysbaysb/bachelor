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

	int fuel;
} Info;

static void worldStatusCallback(const WorldStatus* ws, void* additional);

int L = 0,
	R = 0;

int main(int argc, char** argv)
{
	Info callbackInfo = {0};
	callbackInfo.fuel = 1;

	if(argc < 5) {
		printf("%s host interface L R\n", argv[0]);
		return 0;
	}

	L = std::stoi(argv[3]);
	R = std::stoi(argv[4]);

	callbackInfo.network = new Network(argv[2], 7777, 7777);

	if(!(callbackInfo.worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	callbackInfo.robot = createRobot(callbackInfo.worldCtx);
	printf("Created robot %d\n", callbackInfo.robot);
	startProcessingWorldEvents(callbackInfo.worldCtx, &worldStatusCallback, (void*)&callbackInfo);

	while(callbackInfo.fuel > 0) {
		callbackInfo.network->poll(1);
	}

	detachFromWorld(callbackInfo.worldCtx);
}

static void worldStatusCallback(const WorldStatus* ws, void* additional)
{
	Info* info = (Info*)additional;


	for(size_t i = 0; i < ws->numOfObjects; i++) {
		if(ws->objects[i].type == ROBOT && ws->objects[i].id == info->robot) {
			info->fuel = ws->objects[i].fuel; 

			printf("%lu;%d;(%f:%f);\n", time(nullptr), ws->objects[i].fuel,
					ws->objects[i].x, ws->objects[i].y);

			moveRobot(info->worldCtx, ws->objects[i].id, L, R);
		}
	}
}

