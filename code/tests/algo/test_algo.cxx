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
	auto pos = Vector(0., 0.5f);
	auto c1 = get_nearest_point_on_circle(pos, Vector(0., 0.), 0.5f);
	ASSERT_EQ(pos, c1);

	const auto mid = Vector{0.f, 0.f};
	{
		auto n = get_nearest_point_on_circle(Vector{-1.234f, 0.f}, mid, 1.);
		auto p = Vector{-1., 0.};
		ASSERT_EQ(p, n);
	}
	{
		auto n = get_nearest_point_on_circle(Vector(-2., -2.), mid, 1.);
		auto p = Vector{-0.707107, -0.707107};
		ASSERT_EQ(p, n);
	}
	{
		auto n = get_nearest_point_on_circle(Vector(2.52, -1.92), mid, 1.);
		auto p = Vector{0.77, -0.59};
		ASSERT_EQ(p, n);
	}
}

TEST_F(AlgoTest, RotateTowards) {
	const auto me = Vector(0., 0.);
	const auto rot = 0.;
	
	ASSERT_FLOAT_EQ(rotateTowards(me, rot, Vector(0., 1.)), 180.);
	ASSERT_FLOAT_EQ(rotateTowards(me, rot, Vector(-1., 0.)), 90.);
	ASSERT_FLOAT_EQ(rotateTowards(me, rot, Vector(1., 1.)), 225.);
	ASSERT_FLOAT_EQ(rotateTowards(me, rot, Vector(2., 2.)), 225.);
}

TEST_F(AlgoTest, UnicycleToDiff) {
	ASSERT_EQ(unicycle_to_diff(0, 0), Vector(0, 0));
	ASSERT_EQ(unicycle_to_diff(1, 0), Vector(100, 100));

	auto res = unicycle_to_diff(0, 90);
	ASSERT_TRUE(res.x_ > 0);ASSERT_FLOAT_EQ(res.x_, -res.y_);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}

