/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the basic simulation reside in this file. 
 *
 * More specifically this file tests:
 * 1) can Objects be added to the world?
 * 2) does the collision detection work?
 *
 * All test functions are member functions of WorldTest, which initalizes a
 * new empty world before each test. Most of the the unit tests test "normally",
 * i.e. they expect all function calls to succed. Sometimes a unit test specifically
 * tries to do something illegal to test the error path - to improve clarity
 * the comment explaining that block of code starts with "F:".
 */
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
#include "world.h"

#if 0
TEST(FuelSourceTest, CantMove) {
	// create
	// get pos
	// move
	// oldPos == newPos
}
#endif

class WorldTest: public ::testing::Test { 
public: 
   WorldTest( ) { 
       w = new World(100);
   } 

   #if 0
   void SetUp( ) { 
       // code here will execute just before the test ensues 
   }

   void TearDown( ) { 
       // code here will be called just after the test completes
       // ok to through exceptions from here if need be
   }
   #endif

   ~WorldTest( )  { 
       delete w;
	   // cleanup any pending stuff, but no exceptions allowed
   }

   World* w;
};

TEST_F(WorldTest, AddRobot) {
	/* the world should be empty right after creation */
	auto robotsOld = w->getRobots();
	EXPECT_EQ(robotsOld.size(), 0);
	
	/* add one robot */
	ASSERT_GE(w->addRobot({1, 1}), 0);
	
	/* test */
	auto robotsNew = w->getRobots();
	ASSERT_EQ(robotsNew.size(), 1);
	auto pos = robotsNew.at(0).getPosition();
	EXPECT_EQ(pos.first, 1);
	EXPECT_EQ(pos.second, 1);
}

TEST_F(WorldTest, AddRobotOutsideWorld) {
	auto dimensions = w->getDimensions();

	/* F: outside of the world, right side */
	{
		auto res = w->addRobot({dimensions.first + 1, 1});
		ASSERT_LT(res, 0);
	}
	
	/* F: outside of the world, bottom */
	{
		auto res = w->addRobot({0, dimensions.second + 1});
		ASSERT_LT(res, 0);
	}
}

TEST_F(WorldTest, AddOverlappingRobots) {
	// add
	auto dimensions = w->getDimensions();
	auto x = dimensions.first / 2 ;
	auto res = w->addRobot({x, 1});

	/* F: a second robot completly overlapping the other one */
	{
		auto res = w->addRobot({x, 1});
		ASSERT_LT(res, 0) << " 2nd robot overlapping the 1st was wrongly added";
	}

	/* a second robot, overlapping nothing.
	   This robot is far enough from the first robot but does
	   not leave enough space for another one in between*/
	auto res2 = w->addRobot({x + Robot::DIMENSION + 1, 1});
	EXPECT_GT(res2, res) << " 2nd robot, one robot dimension from the other";

	/* F: add a third robot, overlapping the second one */
	auto res3 = w->addRobot({x + Robot::DIMENSION, 1});
	EXPECT_LT(res3, 0) << "3rd robot, overlapping the 2nd one";
}

TEST_F(WorldTest, AddFuelSource) {
	ASSERT_EQ(w->addFuelSource({0, 0}), 0);
	ASSERT_TRUE(w->getFuelSource());
}

TEST_F(WorldTest, AddFuelSourceTwice) {
	auto res = w->addFuelSource({0, 0});
	ASSERT_EQ(res, 0);
	
	/* F: */
	ASSERT_LT(w->addFuelSource({5, 5}), 0);
}

TEST_F(WorldTest, AddOverlappingRobotAndFuelSource) {
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
