/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the robot control algorithm reside in this file. 
 *
 * More specifically this file tests:
 *
 * All test functions are member functions of AlgoTest. Most of the the unit tests 
 * test "normally",  * i.e. they expect all function calls to succed. Sometimes a unit
 * test specifically  * tries to do something illegal to test the error path - to 
 * improve clarity the comment explaining that block of code starts with "F:".
 */
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <libalgo/algo.h>

class AlgoTest: public ::testing::Test { 
public: 
   AlgoTest( ) { 
   } 

   ~AlgoTest( )  { 
   }

};

TEST_F(AlgoTest, Bla) {
	ASSERT_EQ(0, 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}

