/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the vector class reside in this file. 
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

class VectorTest: public ::testing::Test { 
public: 
   VectorTest( ) { 
   } 

   ~VectorTest( )  { 
   }

};

TEST_F(VectorTest, AddSub) {
	Vector a{1, 0};
	Vector b{0, 2};
	Vector c{1, 2};
	ASSERT_EQ(a + b, c);
	ASSERT_EQ(b + a, c);

	ASSERT_EQ(c - a, b);
	ASSERT_EQ(c - b, a);
}

TEST_F(VectorTest, Length) {
	Vector a{1, 0};
	Vector b{0, 1};
	ASSERT_EQ(a.length(), 1);
	ASSERT_EQ(b.length(), 1);

	auto c = a + b;
	ASSERT_EQ(c.length(), sqrt(2));
}

TEST_F(VectorTest, AngleBetween) {
	Vector a{1, 0};
	Vector b{0, 1};

	ASSERT_FLOAT_EQ(angle(a, b), 90.);
	ASSERT_FLOAT_EQ(angle(a + b, b), 45.);
	ASSERT_FLOAT_EQ(angle(a + b, a), 45.);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}


