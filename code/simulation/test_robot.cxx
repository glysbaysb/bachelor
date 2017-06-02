/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the basic robot functionality reside in this file. 
 *
 * More specifically this file tests:
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
       // todo: add robots and fuelsource
   } 

   ~RobotTest( )  { 
       delete w;
       // cleanup any pending stuff, but no exceptions allowed
   }

   World* w;
};

TEST_F(RobotTest, Test1) {
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}

