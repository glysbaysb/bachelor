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

TEST(RobotTest, Move) {
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
	w->addRobot({1, 1});
	
	/* test */
	auto robotsNew = w->getRobots();
	EXPECT_EQ(robotsNew.size(), 1);
	auto pos = robotsNew.at(0).getPosition();
	EXPECT_EQ(pos.first, 1);
	EXPECT_EQ(pos.second, 1);
}

TEST_F(WorldTest, AddRobotOutsideWorld) {
	/* outside of the world, right side */
	{
		auto dimensions = w->getDimensions();
		auto res = w->addRobot({dimensions.first + 1, 1});
		ASSERT_LT(res, 0);
	}
	
	/* outside of the world, left side */
	{
		auto res = w->addRobot({0, 1});
		ASSERT_LT(res, 0);
	}
}

TEST_F(WorldTest, AddOverlappingRobots) {
	ASSERT_TRUE(false);
	// create
	// add
	// add
}

TEST_F(WorldTest, AddFuelSource) {
	ASSERT_TRUE(false);
	// create
	// add
	// test
}

TEST_F(WorldTest, AddFuelSourceTwice) {
	ASSERT_TRUE(false);
	// create
	// add
	// add
}

TEST_F(WorldTest, AddOverlappingRobotAndFuelSource) {
	ASSERT_TRUE(false);
	// create
	// add
	// add
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}