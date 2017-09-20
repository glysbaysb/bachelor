/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the ECC functionality are in this file
 * The ECC is a RS (X, Y) code
 *
 * The tests are:
 * * 
 *
 * All test functions are member functions of ECCTest.
 * Most of the unit tests test "normally", i.e. they expect all funciton calls
 * to succeed. Sometimes a unit test specifically tries something illegal to
 * test the error path. Those blocks of code are marked "F:" to improve clarity
 */
#include <gtest/gtest.h>
#include <iostream>

#include <libecc/ecc.h>

class ECCTest: public ::testing::Test {
private:
protected:
public:
	ECCTest() {
		
	}
};

TEST_F(ECCTest, TestName) {
	//EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &fake_callback, (void*)0xABCD9876), 0);

//	FAIL() << "can't createRPCRequest()";
	//	ASSERT_TRUE(sizeof(expected) <= outLen);
//		ASSERT_TRUE(memcmp(expected, data, sizeof(expected)) == 0)
//			<< "msgpacked data not as expected";	
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

