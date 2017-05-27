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

static bool doesObjectFitIntoWorld(Position& object, uint32_t sizeOfObject,
	std::pair<uint32_t, uint32_t> sizeOfWorld)
{
	auto p = object.get();
	auto x = p.first;
	auto y = p.second;
	
	if((x - sizeOfObject) < 0 || (y - sizeOfObject) < 0)
		return false;
	
	if((x + sizeOfObject) > sizeOfWorld.first || (y + sizeOfObject) > sizeOfWorld.second)
		return false;
	
	return true;
}

int World::addRobot(Position p)
{
	// todo: does the robot fit into the world?
	if(!doesObjectFitIntoWorld(p, Robot::ROBOT_DIMENSION, getDimensions()))
		return -1;
	
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