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
	// test number
	// add
	// test number
}

TEST_F(WorldTest, AddRobotOutsideWorld) {
	// create
	// add
}

TEST_F(WorldTest, AddOverlappingRobots) {
	// create
	// add
	// add
}

TEST_F(WorldTest, AddFuelSource) {
	// create
	// add
	// test
}

TEST_F(WorldTest, AddFuelSourceTwice) {
	// create
	// add
	// add
}

TEST_F(WorldTest, AddOverlappingRobotAndFuelSource) {
	// create
	// add
	// add
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}