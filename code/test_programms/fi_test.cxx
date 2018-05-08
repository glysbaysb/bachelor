/**
 * @file A simple test application that tries to move the robot for exactly X length units
 *
 * The robot spawns in a random location and then moves randomly in one direction until it is
 * about to fall off. Then it turns 180 degrees and starts to move again. This continues until
 * it has moved X length units.
 */
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

	Vector last;
	float rotation; //! saves the inital rotation as the robot approaches the edge
	bool initalized;

	float distance; //! sum_{i} (curr - last) \\ last = curr

	enum {
		MOVE,
		ROTATE
	} state;

	Info() : robot(0), worldCtx(nullptr), network(nullptr), fuel(0), initalized(false), distance(0.f), last(0, 0) {
	}
};

struct PI {
	float i;
	float e_prev;
	timespec t_prev;

	void clear() {
		i = e_prev = 0.0f;
		clock_gettime(CLOCK_MONOTONIC, &t_prev);
	}
};
PI controller;

static void worldStatusCallback(const WorldStatus* ws, void* additional);

int main(int argc, char** argv)
{
	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}

	Info callbackInfo;

	controller.clear();

	callbackInfo.network = new Network(argv[2], 7777, 7777);

	if(!(callbackInfo.worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	callbackInfo.robot = createRobot(callbackInfo.worldCtx);
	printf("Created robot %d\n", callbackInfo.robot);
	startProcessingWorldEvents(callbackInfo.worldCtx, &worldStatusCallback, (void*)&callbackInfo);

	const auto DISTANCE = 5;
	while(callbackInfo.fuel > 0 && callbackInfo.distance < DISTANCE) {
		callbackInfo.network->poll(1);
	}

	detachFromWorld(callbackInfo.worldCtx);
}

static float clamp(float v, float min, float max)
{
	assert(max > min);

	if(v < min) {
		v = min;
	} else if(v > max) {
		v = max;
	}

	return v;
}

static float PID(float e, PI& pi)
{
	const float P = 0.05f,
		//D = 0.001f,
		I = 0.01f;

	timespec tmp;
	clock_gettime(CLOCK_MONOTONIC, &tmp);
	auto timeFrame = (pi.t_prev.tv_sec - tmp.tv_sec) * 1000 +
		lround(pi.t_prev.tv_nsec / 1.0e6 - pi.t_prev.tv_nsec / 1.0e6);
	clock_gettime(CLOCK_MONOTONIC, &pi.t_prev);

	pi.i += e * timeFrame;
	pi.i = clamp(pi.i, -10, 10);
	float deriv = 0;// (e - pi.e_prev) / timeFrame;
	pi.e_prev = e;

	return e * P + pi.i * I/* + deriv * D*/;
}

/* reuse the static functions. kinda ugly but whatevs */
#include <libalgo/algo.h>
#include <libalgo/algo.cxx>


static void worldStatusCallback(const WorldStatus* ws, void* additional)
{
	const auto TOLERANCE = 0.25f;

	Info* info = (Info*)additional;

	for(size_t i = 0; i < ws->numOfObjects; i++) {
		if(ws->objects[i].type != ROBOT || ws->objects[i].id != info->robot) {
			continue;
		}
		auto me = ws->objects[i];
		const auto myPos = Vector{me.x, me.y};

		/* initalize? */
		if(!info->initalized) {
			info->last = myPos;
		} else {
			info->distance += (myPos - info->last).length();
		}

		/* move */
		printf("%lu:%d", time(nullptr), ws->objects[i].fuel);
		info->fuel = ws->objects[i].fuel; 

		switch(info->state) {
		case Info::MOVE: {
			// todo: is near end? break;

			info->rotation = me.rotation;
			auto m = unicycle_to_diff(1, 0);
			moveRobot(info->worldCtx, ws->objects[i].id, m.x_, m.y_);

			break;
		}
		case Info::ROTATE: {
			// rotation complete?
			if(abs(me.rotation - info->rotation) > 175) {
				break;
			}
			
			auto m = unicycle_to_diff(0, 1);
			moveRobot(info->worldCtx, ws->objects[i].id, m.x_, m.y_);

			break;
	    }
		}
	}
}
