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
#include "world.h"
#include "robot.h"

class RobotTest: public ::testing::Test { 
public: 
   RobotTest( ) { 
       w = new World(100);

       w->addRobot({5, 5});
       w->addRobot({12, 0});
       w->addRobot({-50, -5});
       w->addFuelSource({0, 0});
   } 

   ~RobotTest( )  { 
       delete w;
       // cleanup any pending stuff, but no exceptions allowed
   }

   World* w;
};

if 0
TEST_F(RobotTest, CantMoveWithoutFuel) {
}

TEST_F(RobotTest, CanMoveWithFuel) {
}

TEST_F(RobotTest, MovementBurnsFuel) {
}

TEST_F(RobotTest, IdlingBurnsFuel) {
}

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

