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

#include <libnetwork/network.h>

class NetworkTest: public ::testing::Test {
private:
protected:
public:
	NetworkTest() {
	}

	~NetworkTest() {
	}
};


TEST_F(NetworkTest, Fake) {
	ASSERT_EQ(true, true);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

