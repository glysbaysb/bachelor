/**
 */
#include <stdint.h>
#include <inttypes.h>
#include <memory>
#include <assert.h>
#include <utility>
#include <vector>
#include "world.h"

uint32_t Robot::GLOBAL_ID = 0;

World::World(int8_t dimension)
{	
	dimension_ = dimension;
}

int World::addRobot(Position p)
{
	// todo: does the robot fit into the world?
	
	auto r = Robot(p);
	robots_.push_back(r);
	return r.getID();
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