/**
 */
#include <stdint.h>
#include <inttypes.h>
#include <memory>
#include <assert.h>
#include <utility>
#include <vector>
#include "world.h"

static const int ROBOT_DIMENSION = 1;

World::World(int8_t dimension)
{	
	dimension_ = dimension;
}

int World::addRobot(Position p)
{
	// todo: does the robot fit into the world?
	
	robots_.emplace_back(p);
	return 0;
}

int World::addFuelSource(Position p) {
	fuelSource_ = std::make_unique<FuelSource>(p);
}

float World::getWorldDegree() {
	assert(false);
}

std::vector<Robot> World::getRobots() {
	return robots_;
}