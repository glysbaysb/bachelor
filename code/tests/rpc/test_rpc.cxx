/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the RPC functionality are in this file
 *
 * The tests are:
 * 
 * All test functions are member functions of RPCTest.
 * Most of the unit tests test "normally", i.e. they expect all funciton calls
 * to succeed. Sometimes a unit test specifically tries something illegal to
 * test the error path. Those blocks of code are marked "F:" to improve clarity
 */
#include <gtest/gtest.h>
#include <iostream>

#include <librpc/rpc.h>

class RPCTest: public ::testing::Test {
private:
protected:
	void* rpc;
public:
	RPCTest() {
		if((rpc = createRPCContext()) == NULL) {
			throw "can't create RPC context";
		}
	}
};

void fake_callback(void* optional, int* params) {
	(void)optional;
	(void)params;
}
TEST_F(RPCTest, RegisterProcedure) {
	ASSERT_EQ(addProcedure(rpc, (enum Procedure)1, &fake_callback, (void*)0xABCD9876), 0);

	RPCProcedure procedures = {(enum Procedure)0, NULL, NULL};
	ASSERT_EQ(getRegisteredProcedures(rpc, &procedures, sizeof(procedures)), 0);

	ASSERT_EQ(procedures.num, 1);
	ASSERT_EQ(procedures.optional, (void*)0xABCD9876);
	ASSERT_EQ(procedures.proc, &fake_callback);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
