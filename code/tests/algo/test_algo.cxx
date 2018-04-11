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

TEST_F(AlgoTest, FindCircle) {
	/* Exactly on circle -> no need to move */
	auto pos = Vector{0., 0.5f};
	auto c1 = get_nearest_point_on_circle(pos, Vector{0., 0.}, 0.5f);
	ASSERT_EQ(pos, c1);
	//ASSERT_EQ(Vector{-1., 0.}, get_nearest_point_on_circle(Vector{-1., 0.}, 1.));

	/*
	   (0.894427;0.447214)
	   (0.707107;0.707107)
	   (-0.707107;-0.707107)
	   */

	std::cout << get_nearest_point_on_circle(Vector{2., 1.}) << std::endl;
	std::cout << get_nearest_point_on_circle(Vector{2., 2.}) << std::endl;
	std::cout << get_nearest_point_on_circle(Vector{-2., -2.}) << std::endl;
	std::cout << get_nearest_point_on_circle(Vector{0., 0.}) << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}

