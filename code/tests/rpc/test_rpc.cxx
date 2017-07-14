/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the RPC functionality are in this file
 *
 * The tests are:
 * * adding a callback procedure
 * * creating a rpc request and checking if it conforms to spec
 * * creating a rpc request and checking if it is added to the in flight list
 * * adding a procedure, creating a request and seeing if, as the response is
 *   parsed, the procedure is called
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

TEST_F(RPCTest, CreateRequest) {
	addProcedure(rpc, (enum Procedure)1, &fake_callback, (void*)0xABCD9876);

	int param = 1;
	void* out; size_t outLen;
	if((createRPCRequest(rpc, (enum Procedure)1, &param, 1, &out, &outLen) < 0)) {
		FAIL() << "can't createRPCRequest()";
		return;
	}
	/* check msgpackd data */
	{
		const unsigned char expected[] = {0x94, // arr with 4 elems
			0x00, // operation == REQUEST
			0x01, // ID
			0x01, // procedure
			0x91, // params arr with 1 elem
			0x01 // the elem
		};
		auto data = (const unsigned char*)out;

		ASSERT_TRUE(sizeof(expected) <= outLen);
		// todo: actually parse the messagepack'd data
		ASSERT_TRUE(memcmp(expected, data, sizeof(expected)) == 0)
			<< "msgpacked data not as expected";	
	}
	free(out);
}

TEST_F(RPCTest, CheckInFlight) {
	/* create */
	EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &fake_callback, (void*)0xABCD9876), 0);

	int param = 1;
	void* out; size_t outLen;
	EXPECT_EQ(createRPCRequest(rpc, (enum Procedure)1, &param, 1, &out, &outLen), 0);

	int8_t id = *(int8_t*)(((unsigned char*)out) + 2);
	free(out);

	/* get in flights */
	RPCInFlight inFlight = {0};
	ASSERT_EQ(getRPCsInFlight(rpc, &inFlight, 1), 0);

	/* iterate & check */
	ASSERT_EQ(inFlight.id, id);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
