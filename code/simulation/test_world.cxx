/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the basic simulation reside in this file. 
 *
 * More specifically this file tests:
 * 1) can Objects be added to the world?
 * 2) does the collision detection work?
 * 3) todo: test getRobot()
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
#include <iostream>
#include "world.h"
#include "robot.h"

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
	auto num = w->getNumOfRobots();
	EXPECT_EQ(num, 0);
	
	/* add one robot */
	auto id = w->addRobot({-1, 1});
	ASSERT_GE(id, 0);
	
	/* test */
	auto numAfter = w->getNumOfRobots();
	ASSERT_EQ(numAfter, 1);
	auto pos = w->getRobot(id).getPosition();
	EXPECT_EQ(pos.first, -1);
	EXPECT_EQ(pos.second, 1);
}

TEST_F(WorldTest, AddRobotOutsideWorld) {
	auto dimension = w->getDimension();

	/* F: outside of the world, right side */
	{
		auto res = w->addRobot({dimension + 1, 1});
		ASSERT_LT(res, 0);
	}
	
	/* F: outside of the world, bottom */
	{
		auto res = w->addRobot({0, dimension});
		ASSERT_LT(res, 0);
	}
}

TEST_F(WorldTest, AddOverlappingRobots) {
	// add
	auto dimension = w->getDimension();
	auto x = dimension / 2 ;
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

/**
 * first add the source then the robot
 */
TEST_F(WorldTest, AddOverlappingRobotAndFuelSource1) {
	ASSERT_EQ(w->addFuelSource({0, 0}), 0);
	ASSERT_LT(w->addRobot({0, 1}), 0);
}

/**
 * first add the robot then the source
 */
TEST_F(WorldTest, AddOverlappingRobotAndFuelSource2) {
	ASSERT_GT(w->addRobot({0, 1}), 0);
	ASSERT_LT(w->addFuelSource({0, 0}), 0);
}

TEST_F(WorldTest, CheckPressureVector) {
	/* there is no pressure on an empty world */
	auto v0 = w->getWorldPressureVector();
	ASSERT_EQ(v0.first, 0);
	ASSERT_EQ(v0.second, 0);

	/* add two objects of the same weight the same distance from the mid ... */
	w->addRobot({5, -5});
	w->addRobot({-5, 5});

	/* ... means that the resulting vector is the nullvector */
	auto v1 = w->getWorldPressureVector();
	ASSERT_EQ(v1.first, 0);
	ASSERT_EQ(v1.second, 0);

	/* adding another object anywhere else means that the force is applied unevenly */
	w->addFuelSource({-1, 0});
	auto v2 = w->getWorldPressureVector();
	ASSERT_LT(v2.first, v1.first);
	ASSERT_EQ(v2.second, v2.second);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}
