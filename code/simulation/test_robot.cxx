/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the basic robot functionality reside in this file. 
 *
 * More specifically this file tests:
 * 1) if a robot has run out of fuel it cannot move anymore
 * 2) and conversly: if a robot still has fuel it can move
 * 3) if a robot moves it burns fuel
 * 4) if a robot does not move, it burns less fuel
 * 5) if a robot is told to rotate it rotates in the right direction
 * 6) a robot can only rotate so much per simulation step
 * 7-8) same for movement
 *
 * All test functions are member functions of RobotTest.
 * That class initalizes a world with 3 robots in it before each test.
 * Most of the the unit tests test "normally", i.e. they expect all function
 * calls to succed. Sometimes a unit test specifically tries to do something illegal
 * to test the error path - to improve clarity the comment explaining that block of
 * code starts with "F:".
 */
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <experimental/optional>
#include <chrono>
#include <random>
#include "world.h"
#include "robot.h"

std::minstd_rand0 random_(std::chrono::system_clock::now().time_since_epoch().count());

class RobotTest: public ::testing::Test { 
private:
protected:
	std::vector<uint32_t> ids;	

	uint32_t getRandomRobot() {
		auto it = ids.begin();
		std::advance(it, random_() % ids.size());
   }
public: 
	RobotTest( ) { 
		/* initalize world */
		w = new World(100);

		ids.emplace_back(w->addRobot({5, 5}));
		ids.emplace_back(w->addRobot({12, 0}));
		ids.emplace_back(w->addRobot({-50, -5}));
		w->addFuelSource({0, 0});
	} 

	~RobotTest( )  { 
		delete w;
		// cleanup any pending stuff, but no exceptions allowed
	}

   World* w;
};

#if 0
TEST_F(RobotTest, CantMoveWithoutFuel) {
}
#endif

TEST_F(RobotTest, CanMoveWithFuel) {
	/* get position before */
	auto r = w->getRobot(getRandomRobot());
	auto position = r->getPosition();

	/* change position by moving the robot */
	w->moveRobot(r->getID(), true);
	w->update();
	
	/* compare with old position */
	auto newPosition = r->getPosition();
	ASSERT_NEQ(position, newPosition);
}

TEST_F(RobotTest, MovementBurnsFuel) {
	/* get state before */
	auto r = w->getRobot(getRandomRobot());
	auto fuelBefore = r->getFuelStatus();
	EXPECT_GT(fuelBefore, 0);

	/* change state by moving the robot */
	w->moveRobot(r->getID(), true);
	w->update();
	
	/* compare with old state */
	auto fuelAfter = r->getFuelStatus();
	EXPECT_LT(fuelAfter, fuelBefore);
}

TEST_F(RobotTest, IdlingBurnsFuel) {
	/* get state before */
	auto r = w->getRobot(getRandomRobot());
	auto fuelBefore = r->getFuelStatus();
	EXPECT_GT(fuelBefore, 0);

	/* change state by idling */
	w->update();
	
	/* compare with old state */
	auto fuelAfter = r->getFuelStatus();
	EXPECT_LT(fuelAfter, fuelBefore);
}

#if 0
TEST_F(RobotTest, RotatesInRightDirection) {
}

TEST_F(RobotTest, CantRotateTooMuchPerTurn) {
}

TEST_F(RobotTest, MovesInRightDirection) {
}

TEST_F(RobotTest, CantMoveTooMuchPerTurn) {
}
#endif

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}

