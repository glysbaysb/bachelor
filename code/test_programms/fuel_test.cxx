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

	callbackInfo.network = new Network(argv[2], 7777, 7777);

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

static float PID(float e, float timeFrame, float& integral, float& lastError)
{
	const float P = 1,
		D = 0.001f,
		I = 0.05f;

	integral += e * timeFrame;
	float deriv = 0;// (e - lastError) / timeFrame;
	lastError = e;
	return e * P + integral * I + deriv * D;
}

/* reuse the static functions. kinda ugly but whatevs */
#include <libalgo/algo.h>
#include <libalgo/algo.cxx>



static std::pair<int, int> _move(const Info* const info, const SimulationObject& me)
{
	const auto CIRCLE_RADIUS = 5.0f;
	const auto TOLERANCE = 0.25f;

	const auto myPos = Vector{me.x, me.y};
	/* is in a correct position, i.e. somewhere +-Xm around the middle */
	const bool onCircle = !_isInsideCircle(myPos, CIRCLE_RADIUS - TOLERANCE) &&
		_isInsideCircle(myPos, CIRCLE_RADIUS + TOLERANCE);

	const auto dest = Vector{5, 5};//onCircle ? myPos : get_nearest_point_on_circle({me.x, me.y}, {0., 0.}, CIRCLE_RADIUS);
	const auto len = (dest - myPos).length() * 100.;
	std::cout << "move from " << myPos << " to " << dest << '\t' << len << '\n';
	const auto rot = rotateTowards(myPos, me.rotation, dest);
	std::cout << "current rot " << me.rotation << ". need to turn: " << rot << '\n';

	/* basically facing in the right direction? -> forward */
	if(rot > -5 && rot < 5) {
		// Problem: Vor oder zuruck? Links oder rechts?
		auto ret = unicycle_to_diff(len, rot);
		return {ret.x_, ret.y_};
	} 
	/* else: rotate in place */
	else if(rot < -5 && rot > -180) {
		std::cout << "left\n";
		auto ret = unicycle_to_diff(0, rot);
		return {ret.x_, ret.y_};
	} else if(rot > 5 && rot < 180) {
		std::cout << "right\n";
		auto ret = unicycle_to_diff(0, -rot);
		return {ret.x_, ret.y_};
	} else {
		std::cout << rot << " not handled" << std::endl;
		return {0., 0.};
	}
}

static void worldStatusCallback(const WorldStatus* ws, void* additional)
{
	Info* info = (Info*)additional;

	printf("(%f:%f)\n", ws->xTilt, ws->yTilt);

	for(size_t i = 0; i < ws->numOfObjects; i++) {
		printf("#%d is a %s\n", ws->objects[i].id, ws->objects[i].type == ROBOT ? "robot" : "fuel station");
		printf("\tPos: (%f:%f)\n", ws->objects[i].x, ws->objects[i].y);
		printf("\tMass: %f\n", ws->objects[i].m);
		printf("\tRot: %f\n", ws->objects[i].rotation);

		if(ws->objects[i].type == ROBOT && ws->objects[i].id == info->robot) {
			printf("\tFuel: %d\n", ws->objects[i].fuel);

			auto m = _move(info, ws->objects[i]);
			moveRobot(info->worldCtx, ws->objects[i].id, m.first, m.second);
		}
	}
}



