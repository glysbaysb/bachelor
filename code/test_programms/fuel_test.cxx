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

float PID(float e, float timeFrame, float& integral, float& lastError)
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
	const auto DISTANCE = 1.0f;
	const auto TOLERANCE = 0.05f;

	/* is in a correct position, i.e. somewhere +-Xm around the middle */
	bool onCircle = !_isInsideCircle({me.x, me.y}, DISTANCE - TOLERANCE) &&
		_isInsideCircle({me.x, me.y}, DISTANCE + TOLERANCE);
	if(!onCircle) {
		std::cout << "not on circle\n";
		/* move there */
		auto moveCloser = !_isInsideCircle({me.x, me.y}, DISTANCE + TOLERANCE);
		auto rot = _rotateTowards({me.x, 180. - me.y}, me.rotation, {0., 0.});
		if(moveCloser) {
			if(rot < 5 && rot > 0) {
				std::cout << "gg\n";
				return {100, 100};
			} else if(rot < 180 && rot > 0) {
				std::cout << "left\n";
				return {100, -100};
			} else {
				std::cout << "right\n";
				return {-100, 100};
			}

			std::cout << "closer\n";
			//auto tmp = _unicycle_to_diff({100., });
			//return {tmp.x_, tmp.y_};
		} else {
			/* facing away from middle? */
			if(rot >= 175 && rot <= 185) {
				std::cout << "further\n";
				return {100, 100};
			} else {
				std::cout << "roate\n";
				return {-100, 100};
			}
		}
	} else {
		std::cout << "\n\non the circle\n\n\n";
		/* move on the circle */
		return {0, 0};
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

		if(ws->objects[i].type == ROBOT) {
			printf("\tFuel: %d\n", ws->objects[i].fuel);

			auto m = _move(info, ws->objects[i]);
			moveRobot(info->worldCtx, ws->objects[i].id, m.first, m.second);
		}
	}
}



