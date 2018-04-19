/**
 * @file A simple test application that tries to move the robot along some predefined waypoints
 */
#include <iostream>
#include <iterator>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <libworld/world.h>
#include <libnetwork/network.h>
#include <libalgo/algo.h>

#define WAYPOINTS 20
struct WAYPOINT {
	float phi_;
	float x_;
	float y_;

	WAYPOINT(float phi, float x, float y) {
		phi_ = phi;
		x_ = x;
		y_ = y;
	}
};

typedef struct {
	int robot;
	void* worldCtx;
	Network* network;
	std::vector<WAYPOINT> waypoints;
} Info;

static void worldStatusCallback(const WorldStatus* ws, void* additional);
static std::vector<WAYPOINT> _gen_path(unsigned int);

int main(int argc, char** argv)
{
	Info callbackInfo = {0};
	callbackInfo.waypoints = _gen_path(WAYPOINTS);

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



static std::pair<int, int> _move(const Vector& curr, float rotation, const Vector& dest)
{
	const auto len = (dest - curr).length() * 100.;
	std::cout << "move from " << curr << " to " << dest << '\t' << len << '\n';
	const auto rot = rotateTowards(curr, rotation, dest);
	std::cout << "current rot " << rotation << ". need to turn: " << rot << '\n';

	/* basically facing in the right direction? -> forward */
	if(rot > -5 && rot < 5) {
		std::cout << "accel\n";
		// Problem: Vor oder zuruck? Links oder rechts?
		auto ret = unicycle_to_diff(len, rot);
		return {ret.x_, ret.y_};
	} 
	/* else: rotate in place */
	else if(rot < -5 && rot > -180) {
		std::cout << "left\n";
		auto ret = unicycle_to_diff(0, -rot);
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

static std::pair<int, int> _follow_path(const SimulationObject& me) {
	const auto CIRCLE_RADIUS = 5.0f;
	const auto TOLERANCE = 0.25f;

	const auto myPos = Vector{me.x, me.y};
	/* is in a correct position, i.e. somewhere +-Xm around the middle */
	const bool onCircle = !_isInsideCircle(myPos, CIRCLE_RADIUS - TOLERANCE) &&
		_isInsideCircle(myPos, CIRCLE_RADIUS + TOLERANCE);

	const auto dest = onCircle ? myPos : get_nearest_point_on_circle(myPos, {0., 0.}, CIRCLE_RADIUS);
	return _move(myPos, me.rotation, dest);
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

			auto m = _follow_path(ws->objects[i]);
			moveRobot(info->worldCtx, ws->objects[i].id, m.first, m.second);
		}
	}
}

/**
 * Generates a path (with @cnt edges) along a circle with radius 1
 *
 */
std::vector<WAYPOINT> _gen_path(const unsigned int cnt) {
	std::vector<WAYPOINT> points;

	for(auto i = 0.; i < 360; i += (360. / cnt)) {
		auto degInRad = i * M_PI / 180;

		auto x = sin(degInRad);
		auto y = cos(degInRad);
		points.push_back(WAYPOINT{i, x, y});
	}

	return points;
}
