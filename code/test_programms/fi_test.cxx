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

	PI controller;
	float rotation; //! saves the inital rotation as the robot approaches the edge
	Vector rotPosition; //! position where to robot stopped moving and started rotation,
						//! used to check whether the robot moved far away from that
						//! position to not always oscillate betwween MOVE and ROTATE
	bool initalized;

	Vector last;
	float distance; //! sum_{i} (curr - last) \\ last = curr

	enum {
		MOVE,
		ROTATE,
		MOVE_AWAY,
	} state;

	Info() : robot(0), worldCtx(nullptr), network(nullptr), fuel(1),
		initalized(false), distance(0.f), last(0, 0), rotPosition(0, 0), state(MOVE)
	{
	}
};

static void worldStatusCallback(const WorldStatus* ws, void* additional);

int main(int argc, char** argv)
{
	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}

	Info callbackInfo;
	callbackInfo.controller.clear();

	callbackInfo.network = new Network(argv[2], 7777, 7777);

	if(!(callbackInfo.worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	callbackInfo.robot = createRobot(callbackInfo.worldCtx);
	printf("Created robot %d\n", callbackInfo.robot);
	startProcessingWorldEvents(callbackInfo.worldCtx, &worldStatusCallback, (void*)&callbackInfo);

	const auto DISTANCE = 100;
	while(!callbackInfo.initalized || (callbackInfo.fuel > 0 && callbackInfo.distance < DISTANCE)) {
		callbackInfo.network->poll(1);
	}

	detachFromWorld(callbackInfo.worldCtx);
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
			info->initalized = true;
			info->last = myPos;
		} else {
			info->distance += (myPos - info->last).length();
			info->last = myPos;
		}

		/* move */
		printf("%lu;%d;%f;(%f:%f);\n", time(nullptr), ws->objects[i].fuel, info->distance, myPos.x_, myPos.y_);
		info->fuel = ws->objects[i].fuel; 

		switch(info->state) {
		case Info::MOVE: {
			auto nearEnd = _isInsideCircle(myPos, 10);
			if(!nearEnd) {
				info->rotation = me.rotation;
				info->state = Info::ROTATE;
				break;
			}

			info->rotation = me.rotation;
			auto m = unicycle_to_diff(200, 0);
			moveRobot(info->worldCtx, ws->objects[i].id, m.x_, m.y_);
		}
		break;
		case Info::ROTATE: {
			// rotation complete?
			auto rot = abs(me.rotation - info->rotation);
			if(rot > 175) {
				info->controller.clear();
				info->rotPosition = myPos;
				info->state = Info::MOVE_AWAY;
				break;
			}
			
			auto err = 180 - rot;
			auto m = unicycle_to_diff(0, PID(err, info->controller));
			moveRobot(info->worldCtx, ws->objects[i].id, m.x_, m.y_);
	    }
		break;
		case Info::MOVE_AWAY: {
			if(abs((myPos - info->rotPosition).length()) > 2) {
				info->state = Info::MOVE;
			}
			info->rotation = me.rotation;
			auto m = unicycle_to_diff(200, 0);
			moveRobot(info->worldCtx, ws->objects[i].id, m.x_, m.y_);
	    }
		break;
		}
	}
}
