/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the ficfg functionality are in this file
 *
 * The tests are:
 * - Fake the X- and Y-Angle
 * - Fake the robot rotaiton
 * - Fake the FAULT() macro
 *
 * All test functions are member functions of Ficfg.
 * Most of the unit tests test "normally", i.e. they expect all funciton calls
 * to succeed. Sometimes a unit test specifically tries something illegal to
 * test the error path. Those blocks of code are marked "F:" to improve clarity
 *
 * As the libficfg heavily uses rand() there is a "mock" of that function that
 * outputs a well known sequence, carefully (*cough*) crafted to ensure
 * determinism.
 *
 * @todo: FakeMass(), FakeFuel()
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
	return sequence[idx++];
}

class FicfgTest : public ::testing::Test {
private:
	void new_simobject(SimulationObject* so) {
		so->x = so->y = 0.;

		so->rotation = 90.;
		so->fuel = 500;
		so->m = 100;
	}

	WorldStatus* new_worldstatus() {
		auto ws = (WorldStatus*)calloc(1, sizeof(WorldStatus));
		ws->xTilt = 1.;
		ws->yTilt = 1.;

		ws->numOfObjects = 1;
		ws->objects = (SimulationObject*)calloc(1, sizeof(SimulationObject));
		new_simobject(&ws->objects[0]);

		return ws;
	}

protected:
	WorldStatus* ws_;
	FiCfg cfg;
public:
	FicfgTest () : ws_(new_worldstatus())
	{
		cfg.dropWorldStatus = 1000;
		cfg.dupWorldStatus = 1000;
		cfg.fakeWorldStatus = 1000;

		idx = 0;
	}

	~FicfgTest () {
	}
};

/**
 * Tests the FAULT() macro.
 *
 * The first test checks for the special case where a probability is
 * zero -> it never evaluates to true.
 *
 * Then there are a few tests whether FAULT() adheres to the basic
 * probabilty. First some dumb tests, then whether it returns true 2 times
 * for 3 invocations with a 50% change (and a rigged PRNG)
 */
TEST_F(FicfgTest, ProbabilityCfg) {
	ASSERT_EQ(FAULT(0), 0);	

	static const int s[] = {700, 1000};
	sequence = s;

	{
		idx = 0;
		ASSERT_EQ(FAULT(cfg.dropWorldStatus), 0);
		ASSERT_EQ(FAULT(cfg.dropWorldStatus), 1);
	}

	{
		idx = 0;
		ASSERT_EQ(FAULT(cfg.dupWorldStatus), 0);
		ASSERT_EQ(FAULT(cfg.dupWorldStatus), 1);
	}

	{
		idx = 0;
		ASSERT_EQ(FAULT(cfg.fakeWorldStatus), 0);
		ASSERT_EQ(FAULT(cfg.fakeWorldStatus), 1);
	}

	{
		static const int s[] = {1, 0, 1};
		sequence = s;

		idx = 0;
		ASSERT_EQ(FAULT(2), 0);
		ASSERT_EQ(FAULT(2), 1);
		ASSERT_EQ(FAULT(2), 0);
	}
}

/**
 * Tests whether the tilt angle is faked correctly
 *
 * todo: test whether the implementation adheres to max change
 */
TEST_F(FicfgTest, FakeAngle) {
	static const int s[] = {0, // sign
		1, // type
		15000, // change

		0, // sign
		2, // type
		15000, // change
	};
	sequence = s;

	EXPECT_FLOAT_EQ(ws_->xTilt, 1);
	EXPECT_FLOAT_EQ(ws_->yTilt, 1);

	fakeWorldStatus(ws_);
	ASSERT_FLOAT_EQ(ws_->xTilt, 2.5);

	fakeWorldStatus(ws_);
	ASSERT_FLOAT_EQ(ws_->yTilt, 2.5);
}

/**
 * Tests whether the robot position is faked correctly
 *
 * todo: test whether the implementation adheres to max change
 */
TEST_F(FicfgTest, FakePosition) {
	static const int s[] = {1, // sign
		4, // type
		1, // index
		1, // sign
		0, // type
		45000, // change

		1, // sign
		4, // type
		1, // index
		1, // sign
		1, // type
		45000, // change
	};
	sequence = s;

	fakeWorldStatus(ws_);
	ASSERT_FLOAT_EQ(ws_->objects[0].x, -4.5);

	fakeWorldStatus(ws_);
	ASSERT_FLOAT_EQ(ws_->objects[0].y, -4.5);
}

/**
 * Tests whether the robot rotation is faked correctly
 *
 * todo: test whether the implementation adheres to max change
 */
TEST_F(FicfgTest, FakeRotation) {
	static const int s[] = {1, // sign
		4, // type
		0, // index
		1, // sign
		2, // type
		20000, // change

		0,
		4,
		0,
		0,
		2,
		20000,
	};
	sequence = s;

	/* down to -2 */
	fakeWorldStatus(ws_);
	ASSERT_FLOAT_EQ(ws_->objects[0].rotation, 88.);

	{
		for(auto i = 0; i < 44; i++) {
			idx = 0;
			fakeWorldStatus(ws_);
		}

		EXPECT_FLOAT_EQ(ws_->objects[0].rotation, 0.);


		idx = 0;
		fakeWorldStatus(ws_);
		ASSERT_FLOAT_EQ(ws_->objects[0].rotation, -2.);
	}


	/* up again */
	idx = 6;
	fakeWorldStatus(ws_);
	ASSERT_FLOAT_EQ(ws_->objects[0].rotation, 0.);

}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
