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

#include <msgpack.h>
#include <librpc/rpc.h>

class RPCTest: public ::testing::Test {
private:
protected:
	void* rpc;
public:
	RPCTest() {
		if((rpc = createRPCContext()) == nullptr) {
			throw "can't create RPC context";
		}
	}

	~RPCTest() {
		destroyRPCContext(rpc);
	}
};

static void _fake_callback(void* optional, msgpack_object_array* params);
static void _fake_callback(void* optional, msgpack_object_array* params) {
	(void)optional;
	(void)params;
}
TEST_F(RPCTest, RegisterProcedure) {
	ASSERT_EQ(addProcedure(rpc, (enum Procedure)1, &_fake_callback, (void*)0xABCD9876), 0);

	RPCProcedure procedures = {(enum Procedure)0, nullptr, nullptr};
	ASSERT_EQ(getRegisteredProcedures(rpc, &procedures, sizeof(procedures)), 0);

	ASSERT_EQ(procedures.num, 1);
	ASSERT_EQ(procedures.optional, (void*)0xABCD9876);
	ASSERT_EQ(procedures.proc, &_fake_callback);
}

TEST_F(RPCTest, CreateRequest) {
	addProcedure(rpc, (enum Procedure)1, &_fake_callback, (void*)0xABCD9876);

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
	EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &_fake_callback, (void*)0xABCD9876), 0);

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

static void _set_opt_to_params(void* optional, msgpack_object_array* params);
static void _set_opt_to_params(void* optional, msgpack_object_array* params) {
	ASSERT_EQ(params->size, 1);

	int* o = (int*)optional;
	*o = (int)(params->ptr[0].via.i64 & INT32_MAX);
}

TEST_F(RPCTest, CheckHandleRPC) {
	int changedByRPC = 0;
	const uint8_t magic = 42;

	/* create */
	EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &_set_opt_to_params, (void*)&changedByRPC), 0);

	int param = 1;
	void* out; size_t outLen;
	EXPECT_EQ(createRPCRequest(rpc, (enum Procedure)1, &param, 1, &out, &outLen), 0);

	auto id = *(int8_t*)(((unsigned char*)out) + 2);//todo: that's a really fragile way to get the id
	free(out);

	/* handle reply */
	const unsigned char reply[] = {0x94,
		0x01, // Reply
		id,
		0x00, // err
		0x91, // params arr
		magic
	};

	ASSERT_EQ(handleRPC(rpc, (const char*)reply, sizeof(reply)), 0);
	ASSERT_EQ(changedByRPC, magic);
}

#if 0
void set_opt_to_params(void* optional, int* params) {
	int* o = (int*)optional;
	*o = params[0];
}

TEST_F(RPCTest, CheckHandle) {
	int changedByRPC = 0;
	const uint8_t magic = 42;

	/* create */
	EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &set_opt_to_params, (void*)&changedByRPC), 0);

	int param = 1;
	void* out; size_t outLen;
	EXPECT_EQ(createRPCRequest(rpc, (enum Procedure)1, &param, 1, &out, &outLen), 0);

	uint8_t id = *(int8_t*)(((unsigned char*)out) + 2);
	free(out);

	/* handle reply */
	const unsigned char reply[] = {0x94,
		0x01, // Reply
		id,
		0x00, // err
		0x91, // params arr
		magic
	};

	ASSERT_EQ(handleRPC(rpc, (const char*)reply, sizeof(reply)), 0);
	ASSERT_EQ(changedByRPC, magic);
}
#endif

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
