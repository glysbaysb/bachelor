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
#include <simulation/world.h>
#include <simulation/robot.h>

std::minstd_rand0 random_(std::chrono::system_clock::now().time_since_epoch().count());

class RobotTest: public ::testing::Test { 
private:
protected:
	std::vector<uint32_t> ids;	

	uint32_t getRandomRobot() {
		auto it = ids.begin();
		std::advance(it, random_() % ids.size());
		return *it;
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

TEST_F(RobotTest, CantMoveWithoutFuel) {
	auto id = getRandomRobot();
	/* get position before */
	auto r = w->getRobot(id);

	/* use up all fuel, by moving back and forth */
	uint32_t i = 0;
	while(r->getFuelStatus()) {
		if(i % 10 == 0)
			std::cout << i << ':' << r->getFuelStatus() << '\r';
		int32_t dir[] = {1, 0, -1};
		w->moveRobot(r->getID(), {0, dir[i++]});
		w->update();

		r = w->getRobot(id);
	}

	auto position = r->getPosition();
	/* stop */
	{
		w->moveRobot(r->getID(), {0, 0});
		w->update();

		r = w->getRobot(id);
	}

	/* F: try to move */
	w->moveRobot(r->getID(), {-1, 0});
	w->update();
	r = w->getRobot(id);

	auto newPosition = r->getPosition();
	ASSERT_EQ(position, newPosition);

}

TEST_F(RobotTest, CanMoveWithFuel) {
	auto id = getRandomRobot();
	/* get position before */
	auto r = w->getRobot(id);
	auto position = r->getPosition();

	/* change position by moving the robot */
	w->moveRobot(r->getID(), {0, 1});
	w->update();
	
	/* compare with old position */
	r = w->getRobot(id);
	auto newPosition = r->getPosition();
	ASSERT_NE(position, newPosition);
}

TEST_F(RobotTest, MovementBurnsFuel) {
	auto id = getRandomRobot();
	/* get state before */
	auto r = w->getRobot(id);
	auto fuelBefore = r->getFuelStatus();
	EXPECT_GT(fuelBefore, 0);

	/* change state by moving the robot */
	w->moveRobot(r->getID(), {0, 1});
	w->update();
	
	/* compare with old state */
	r = w->getRobot(id);
	auto fuelAfter = r->getFuelStatus();
	EXPECT_LT(fuelAfter, fuelBefore);
}

TEST_F(RobotTest, IdlingBurnsFuel) {
	auto id = getRandomRobot();
	/* get state before */
	auto r = w->getRobot(id);
	auto fuelBefore = r->getFuelStatus();
	EXPECT_GT(fuelBefore, 0);

	/* change state by idling */
	w->update();
	
	/* compare with old state */
	r = w->getRobot(id);
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

