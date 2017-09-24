/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the network functionality are in this file
 *
 * The tests are:
 * * does the timeout work?
 * * does it send messages?
 * * and recv them?
 * * the echo test with multiple messages
 *
 * Maybe come up with a test about maximally allowed UDP packet sizes?
 *
 * All test functions are member functions of NetworkTest.
 * Most of the unit tests test "normally", i.e. they expect all funciton calls
 * to succeed. Sometimes a unit test specifically tries something illegal to
 * test the error path. Those blocks of code are marked "F:" to improve clarity
 */
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include <libnetwork/network.h>

class NetworkTest: public ::testing::Test {
private:
protected:
	ECCUDP network;
public:
	NetworkTest() : network("7777", "7777") {
	}

	~NetworkTest() {
	}
};


TEST_F(NetworkTest, SendTest) {
	std::vector<uint8_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	ASSERT_EQ(network.send(data), 0);
}

TEST_F(NetworkTest, EchoTest) {
	/* first send something */
	std::vector<uint8_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	EXPECT_EQ(network.send(data), 0);

	/* try to receive it */
	auto packets = std::vector<Packet>();
	network.poll(0, packets);

	/* compare */
	ASSERT_EQ(packets.size(), 1);
	ASSERT_EQ(data, packets.at(0));
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

