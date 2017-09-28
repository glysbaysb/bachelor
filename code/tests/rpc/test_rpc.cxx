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
 *   todo: wait, that test is weird. Where do the parameters go?
 * * creating a RPC request with a complex parameter list and checking if that
 *   is deserialized correctly
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

	void* out; size_t outLen;
	if((createRPCRequest(rpc, (enum Procedure)1, nullptr, 0, &out, &outLen) < 0)) {
		FAIL() << "can't createRPCRequest()";
		return;
	}
	/* check msgpackd data */
	{
		const unsigned char expected[] = {0x94, // arr with 4 elems
			0x00, // operation == REQUEST
			0x01, // ID
			0x01, // procedure
			0x90, // empty params arr
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

	void* out; size_t outLen;
	EXPECT_EQ(createRPCRequest(rpc, (enum Procedure)1, nullptr, 0, &out, &outLen), 0);

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

	void* out; size_t outLen;
	EXPECT_EQ(createRPCRequest(rpc, (enum Procedure)1, nullptr, 0, &out, &outLen), 0);

	auto id = *(uint8_t*)(((unsigned char*)out) + 2);//todo: that's a really fragile way to get the id
	free(out);

	/* handle reply */
	const unsigned char reply[] = {0x94,
		0x01, // Reply
		id,
		0x00, // err
		0x91, // params arr
		magic
	};

	ASSERT_EQ(handleRPC(rpc, reply, sizeof(reply)), 0);
	ASSERT_EQ(changedByRPC, magic);
}

void echo(void* optional, msgpack_object_array* params) {
	(void) optional;
	ASSERT_EQ(params->size, 3);
	ASSERT_EQ(params->ptr[0].via.u64, 0x01234567);
	ASSERT_FLOAT_EQ(params->ptr[1].via.f64, 1234.56);
	ASSERT_STREQ(params->ptr[2].via.str.ptr, "Hello");
}

TEST_F(RPCTest, ComplexParams) {
	/* create */
	EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &echo, nullptr), 0);

	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 3);
	msgpack_pack_int(&pk, 0x01234567);
	msgpack_pack_float(&pk, 1234.56);

	msgpack_pack_str(&pk, 6);
	msgpack_pack_str_body(&pk, "Hello", 6);

	void* out; size_t outLen;
	EXPECT_EQ(createRPCRequest(rpc, (enum Procedure)1, sbuf.data, sbuf.size, &out, &outLen), 0);

	ASSERT_EQ(handleRPC(rpc, (const unsigned char*)out, outLen), 0);
	free(out);
	// todo: free sbuf
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
