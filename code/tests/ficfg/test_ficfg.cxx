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

#include <msgpack.h>
#include <libworld/world.h>
#include <libficfg/ficfg.cxx>

static const int* sequence;
static int idx;
int rand() {
	auto a = sequence[idx++];

	std::cout << a << '\n';
	return a;
}

class FicfgTest : public ::testing::Test {
private:
	void new_simobject(SimulationObject* so) {
		so->x = so->y = 1.;

		so->rotation = 90.;
		so->fuel = 500;
		so->m = 100;
	}

protected:
	WorldStatus* ws;
public:
	FicfgTest (){
		ws = (WorldStatus*)calloc(1, sizeof(ws));
		ws->xTilt = 1.;
		ws->yTilt = 1.;

		ws->numOfObjects = 1;
		ws->objects = (SimulationObject*)calloc(1, sizeof(SimulationObject));
		new_simobject(&ws->objects[0]);

		idx = 0;
	}

	~FicfgTest () {
	}
};


TEST_F(FicfgTest, FakeAngle) {
	static const int s[] = {0, // sign
		1, // type
		15000, // change

		0, // sign
		2, // type
		15000, // change
	};
	sequence = s;

	EXPECT_FLOAT_EQ(ws->xTilt, 1);
	EXPECT_FLOAT_EQ(ws->yTilt, 1);

	fakeWorldStatus(ws);
	ASSERT_FLOAT_EQ(ws->xTilt, 2.5);

	fakeWorldStatus(ws);
	ASSERT_FLOAT_EQ(ws->yTilt, 2.5);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

