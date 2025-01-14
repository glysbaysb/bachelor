/**
 * @file A simple test application that tries to move the robot along some predefined waypoints
 *
 * The Info structure is passed to the worldStatusCallback() which makes the robot follow the path
 * defined in Info::waypoints. When the robot is instantied it probably is not on the path, so it
 * moves to the nearest way point. Once that point has almost been reached the robot follows along
 * the path.
 *
 * Following the path means driving successively to each of the waypoints. For simplicity we assume
 * that the robot has reached the waypoint once it is in a certain distance. However consider this:
 * The robot moves towards A and reaches it finally. Now it should move to point B, but point A is
 * nearer (as the robot actually is at A) and thus will always move back towards A. It essentially
 * becomes stuck.
 * To circumvent that the Info structure has a member Info::nextWaypoint. The robot always is moved
 * towards that. That member is only updated if:
 * a) the robot is not on (/near) the path
 * b) a new waypoint has been reached
 */
#include <iostream>
#include <iterator>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <libworld/world.h>
#include <libnetwork/network.h>
#include <libalgo/algo.h>

const auto CIRCLE_RADIUS = 5.0f;

typedef struct {
	int robot;
	void* worldCtx;
	Network* network;

	std::vector<WAYPOINT> waypoints;
	WAYPOINT nextWaypoint; 

	int fuel;
} Info;

struct ControllerTuple {
	PI accel;
	PI left;
	PI right;

	void clear() {
		accel.clear();
		left.clear();
		right.clear();
	}
};

static struct {
	ControllerTuple  onCircle;
	ControllerTuple  toCircle;

} controllers;

static void worldStatusCallback(const WorldStatus* ws, void* additional);

int main(int argc, char** argv)
{
	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}

	Info callbackInfo = {0};
	callbackInfo.fuel = 1;
	callbackInfo.waypoints = gen_path(WAYPOINTS, CIRCLE_RADIUS);

	controllers.onCircle.clear();
	controllers.toCircle.clear();


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

/* reuse the static functions. kinda ugly but whatevs */
#include <libalgo/algo.h>
#include <libalgo/algo.cxx>


static std::pair<int, int> _move(const Vector& curr, float rotation, const Vector& dest, ControllerTuple& pis)
{
	const auto len = (dest - curr).length() * 100.;
	std::cout << "move from " << curr << " to " << dest << '\t' << len << '\n';
	const auto rot = rotateTowards(curr, rotation, dest);
	std::cout << "current rot " << rotation << ". need to turn: " << rot << '\n';

	/* basically facing in the right direction? -> forward */
	if(rot > -5 && rot < 5) {
		std::cout << "accel\n";
		pis.left.clear(); pis.right.clear();

		// Problem: Vor oder zuruck? Links oder rechts?
		auto ret = unicycle_to_diff(PID(len, pis.accel), rot);
		return {ret.x_, ret.y_};
	} 
	/* else: rotate in place */
	else if(rot < -5 && rot > -180) {
		std::cout << "left\n";
		pis.accel.clear(); pis.right.clear();

		auto ret = unicycle_to_diff(0, PID(rot, pis.left));
		return {ret.x_, ret.y_};
	} else if(rot > 5 && rot < 180) {
		std::cout << "right\n";
		pis.accel.clear(); pis.left.clear();

		auto ret = unicycle_to_diff(0, PID(rot, pis.right));
		return {ret.x_, ret.y_};
	} else {
		std::cout << rot << " not handled" << std::endl;
		return {0., 0.};
	}
}


static std::pair<int, int> _follow_path(const SimulationObject& me, const std::vector<WAYPOINT>& path, WAYPOINT& nearestWaypoint) {
	const auto TOLERANCE = 0.25f;

	const auto myPos = Vector{me.x, me.y};
	/* is in a correct position, i.e. somewhere +-Xm around the middle */
	const bool onCircle = !_isInsideCircle(myPos, CIRCLE_RADIUS - TOLERANCE) &&
		_isInsideCircle(myPos, CIRCLE_RADIUS + TOLERANCE);

	/* if not on circle, move there */
	if(!onCircle) {
		controllers.onCircle.clear(); // clear other controller, to ready it for next use

		nearestWaypoint = get_nearest_waypoint(myPos, path);
		std::cout << "get to circle " << nearestWaypoint << std::endl;

		return _move(myPos, me.rotation, nearestWaypoint, controllers.toCircle);
	}
	/* else follow path */
	else {
		controllers.toCircle.clear(); // see above

		/* point reached? -> find next */
		auto dist = (nearestWaypoint - myPos).length();
		if(dist < TOLERANCE) {
			std::cout << "next one after " << nearestWaypoint << std::endl;

			size_t currIdx = -1;
			for(size_t i = 0; i < path.size(); i++) {
				if(nearestWaypoint == path[i]) {
					currIdx = i;
					break;
				}
			}

			nearestWaypoint = path[(currIdx + 1) % path.size()];
		}

		/* go there */
		return _move(myPos, me.rotation, nearestWaypoint, controllers.onCircle);
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
			info->fuel = ws->objects[i].fuel; 

			auto m = _follow_path(ws->objects[i], info->waypoints, info->nextWaypoint);
			moveRobot(info->worldCtx, ws->objects[i].id, m.first, m.second);
		}
	}
}
