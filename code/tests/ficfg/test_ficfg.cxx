/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the ficfg functionality are in this file
 *
 * The tests are:
*
 * All test functions are member functions of Ficfg.
 * Most of the unit tests test "normally", i.e. they expect all funciton calls
 * to succeed. Sometimes a unit test specifically tries something illegal to
 * test the error path. Those blocks of code are marked "F:" to improve clarity
 */
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <chrono>

#include <libficfg/ficfg.h>

class FicfgTest : public ::testing::Test {
private:
protected:
public:
	FicfgTest (){
	}

	~FicfgTest () {
	}
};


TEST_F(FicfgTest, Simple) {
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

