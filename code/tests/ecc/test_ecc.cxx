/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the ECC functionality are in this file
 * The ECC is a RS (X, Y) code
 *
 * The tests are:
 * * all single byte-errors
 * * a few multiple byte errors
 *
 * All test functions are member functions of ECCTest.
 * Most of the unit tests test "normally", i.e. they expect all funciton calls
 * to succeed. Sometimes a unit test specifically tries something illegal to
 * test the error path. Those blocks of code are marked "F:" to improve clarity
 */
#include <gtest/gtest.h>
#include <iostream>

#include <libecc/ecc.h>

#define MSG_LENGTH (255 - NPAR)
#define RECVD_MSG_LENGTH (MSG_LENGTH + NPAR)

class ECCTest: public ::testing::Test {
private:
	void generate_data(uint8_t* buff, size_t cnt) {
		for(size_t i = 0; i < cnt; i++) {
			buff[i] = i % 256;
		}
	}

protected:
	uint8_t message[MSG_LENGTH];
public:
	ECCTest() {
		initialize_ecc();
		generate_data(message, sizeof(message));
	}
};

TEST_F(ECCTest, SingleByteErrorsTest) {
	uint8_t  codeword[RECVD_MSG_LENGTH];
	encode_data(message, MSG_LENGTH, codeword);

	for(size_t i = 0; i < sizeof(codeword); i++) {
		uint8_t copy[sizeof(codeword)];
		memcpy(copy, codeword, sizeof(copy));

		copy[i] ^= 0xAA;

		decode_data(copy, RECVD_MSG_LENGTH);
		if(check_syndrome () != 0) {
			correct_errors_erasures (copy, 
				 RECVD_MSG_LENGTH,
				 0, 
				 NULL);
		}

		ASSERT_TRUE(memcmp(copy, message, sizeof(message)) == 0)
			<< "decoding error when corrupting position " << i;	
	}
	//EXPECT_EQ(addProcedure(rpc, (enum Procedure)1, &fake_callback, (void*)0xABCD9876), 0);

//	FAIL() << "can't createRPCRequest()";
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

