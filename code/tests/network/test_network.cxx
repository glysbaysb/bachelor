/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the network functionality are in this file
 *
 * The tests are:
 * * does the timeout work?
 * * does it send messages?
 * * and recv them?
 *
 * Todo:
 * * if nothing was sent, is nothing received?
 * * the echo test with multiple messages
 * * a test that could corrupt the message while sending it would be cool
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
#include <chrono>

#include <libnetwork/network.h>

class NetworkTest: public ::testing::Test {
private:
protected:
	ECCUDP network;
public:
	NetworkTest() : network(7777, 7777) {
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
	/* if the network library bound to multiple sockets, make sure that
	   it recved the same thing on all of them */
	ASSERT_GE(packets.size(), 1);
	ASSERT_EQ(data, packets.at(0));
	if(packets.size() > 1) {
		for(auto i = 1; i < packets.size(); i++) {
			ASSERT_EQ(packets.at(0), packets.at(i));
		}
	}
}

TEST_F(NetworkTest, TimeoutTest)
{
	const auto TIMEOUT = 25;
	const auto TIMES = 100;
	auto totalTime = std::chrono::duration<float>();

	auto packets = std::vector<Packet>();

	for(auto i = 0; i < TIMES; i++) {
		auto before = std::chrono::system_clock::now();
		network.poll(TIMEOUT, packets);
		auto after = std::chrono::system_clock::now();

		totalTime += after - before;
	}

	ASSERT_EQ(packets.size(), 0); // as no packets were sent, none should have been retrieved

	auto measured = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime / (double)TIMES);
	auto timeout = std::chrono::milliseconds(TIMEOUT);
	ASSERT_EQ(timeout, measured);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

