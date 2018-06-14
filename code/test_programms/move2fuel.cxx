/**
 * @file A simple test application that tries to move the robot towards the
 * fuel station
 **/
#include <iostream>
#include <iterator>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <libworld/world.h>
#include <libnetwork/network.h>
#include <libalgo/algo.h>


struct Info {
	int robot;
	void* worldCtx;
	Network* network;

	int fuel;

	Vector fuelStation;

	Info() :
		robot(0), worldCtx(nullptr), network(nullptr), fuel(1), fuelStation(0, 0)
	{
	}
};

static void worldStatusCallback(const WorldStatus* ws, void* additional);

int main(int argc, char** argv)
{
	Info callbackInfo;

	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}


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
		if(ws->objects[i].type != ROBOT) {
			info->fuelStation = Vector(ws->objects[i].x, ws->objects[i].y);
		}
		else if(ws->objects[i].type == ROBOT && ws->objects[i].id == info->robot) {
			info->fuel = ws->objects[i].fuel; 

			printf("%lu;%d;(%f:%f);\n", time(nullptr), ws->objects[i].fuel,
					ws->objects[i].x, ws->objects[i].y);

			auto my = Vector(ws->objects[i].x, ws->objects[i].y);

			auto phi = rotateTowards(my, ws->objects[i].rotation, info->fuelStation);
			auto len = (info->fuelStation - my).length();

			auto move = unicycle_to_diff(len, phi);
			moveRobot(info->worldCtx, ws->objects[i].id, move.x_, move.y_);
		}
	}
}
