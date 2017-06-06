/**
 */
#include <iostream>
#include <stdint.h>
#include <inttypes.h>
#include <memory>
#include <assert.h>
#include <utility>
#include <vector>
#include <experimental/optional>
#include <math.h>
#include "world.h"
#include "robot.h"


static bool doesObjectFitIntoWorld(const Position& object, const int32_t sizeOfObject,
	const uint32_t sizeOfWorld)
{
	auto p = object.get();
	auto x = p.first;
	auto y = p.second;
	
	/* the coordinate system starts in the middle of the round plate.
	   abs() reflects the object on the X or Y axis. If that value plus the size of the
	   object is bigger than the size of the world the object does not fit*/
	if((abs(x) + sizeOfObject) > sizeOfWorld || (abs(y) + sizeOfObject) > sizeOfWorld)
		return false;
	
	return true;
}

static bool doObjectsOverlap(const Object& a, const Object& b)
{
	// https://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other
	// if (RectA.Left < RectB.Right && RectA.Right > RectB.Left &&
	//     RectA.Top > RectB.Bottom && RectA.Bottom < RectB.Top )
	// adapted for the different coordinate system, so flip the comparsions on the Y-axis
	bool t1 = (a.getPosition().first < (b.getPosition().first + b.getDimension()));
	bool t2 = ((a.getPosition().first + a.getDimension()) > b.getPosition().first);
	bool t3 = (a.getPosition().second < (b.getPosition().second + b.getDimension()));
	bool t4 = ((a.getPosition().second + a.getDimension()) > b.getPosition().second);

	return t1 && t2 && t3 && t4;
}

int World::doesObjectOverlapWithRobots(const Object& a) const
{
	for(auto&& x : robots_) {
		if(doObjectsOverlap(x, a)) {
			return -1;
		}
	}

	return 0;
}

int World::addRobot(const Position& p)
{
	/* does the robot fit into the world? */
	if(!doesObjectFitIntoWorld(p, Robot::DIMENSION, getDimension()))
		return -1;
	
	auto r = RobotWithRoundInformation(p);
	if((doesObjectOverlapWithRobots(r) < 0))
		return -2;
	if(getFuelSource() && doObjectsOverlap(*getFuelSource(), r))
		return -3;

	/* add */
	robots_.push_back(r);
	return r.getID();
}

FuelSource* World::getFuelSource() const
{
	return fuelSource_.get();
}

int World::addFuelSource(const Position& p) {
	FuelSource tmp(p); // todo: kinda hacky to always construct a temp object...
	if(fuelSource_ || doesObjectOverlapWithRobots(tmp))
		return -1;

	fuelSource_ = std::make_unique<FuelSource>(p);
	return 0;
}

std::pair<int32_t, int32_t> World::getWorldPressureVector() const {
	int32_t vectorX = 0, // the sum of the x part all forces --> for the left-right angle
		vectorY = 0; // the sum of the y part of all forces -> for the front-back angle

	for(auto&& r : robots_) {
		vectorX += r.getPosition().first * r.getWeight();
		vectorY += r.getPosition().second * r.getWeight();
	}

	if(getFuelSource()) {
		auto f = *getFuelSource();
		vectorX += f.getPosition().first * f.getWeight();
		vectorY += f.getPosition().second * f.getWeight();
	}

	return {vectorX, vectorY};
}

int World::moveRobot(const int32_t robot, const Position& diffVector) {
	for(auto&& r : robots_) {
		if(r.getID() != robot)
			continue;

		r.safeMove(diffVector);	
		return 0;
	}

	return -1;
}


void World::update() {
	for(auto&& r : robots_) {
		/* move all robots and prepare them so they can be used again next round */
		r.safeUpdate();
		
		/* todo: collision */
		/* but what if a collision happens? Do the robots crash into eac other
		   and then in the next round crash even further? Or do they bounce
		   back to their old positions?
		or maybe: 
			new = r.update();
			if(!collision)
				r.apply(new);
		*/
		
		/* todo: if near fuelSource: charge */
	}
}

