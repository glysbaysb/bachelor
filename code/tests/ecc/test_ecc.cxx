/**
 * @file
 * @section DESCRIPTION
 * The unit tests for the ECC functionality are in this file
 * The ECC is a RS (X, Y) code
 *
 * The tests are:
 * * all single byte-errors
 * * NPAR burst errors in all possible positions
 * * striding error pattern
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
}

TEST_F(ECCTest, SingleByteErrorsCppTest) {
	const auto message_ = std::vector<uint8_t>(message, message + sizeof(message));
	auto v = message_;
	auto codeword = ECC::encode(v);

	for(size_t i = 0; i < codeword.size(); i++) {
		auto copy = codeword;

		copy[i] ^= 0xAA;

		auto decoded = ECC::decode(copy);

		ASSERT_EQ(decoded, message_)
			<< "decoding error when corrupting position " << i;
	}
}

/*
 * @brief corrupt the maximally allowed amount of bytes (NPAR / 2)
 *        as a burst error.
 *
 *        So [0] to [NPAR/2], then [1] to [NPAR/2 + 1], ...
 */
TEST_F(ECCTest, BurstErrorsTest) {
	const size_t ERRORS = NPAR / 2;
	uint8_t  codeword[RECVD_MSG_LENGTH];
	encode_data(message, MSG_LENGTH, codeword);

	for(size_t i = 0; i < sizeof(codeword) - ERRORS; i++) {
		uint8_t copy[sizeof(codeword)];
		memcpy(copy, codeword, sizeof(copy));

		for(size_t j = 0; j < ERRORS; j++) {
			copy[i + j] ^= 0xA5;
		}

		decode_data(copy, RECVD_MSG_LENGTH);
		EXPECT_GT(check_syndrome(), 0);
		correct_errors_erasures (copy, RECVD_MSG_LENGTH, 0, NULL);

		ASSERT_TRUE(memcmp(copy, message, sizeof(message)) == 0)
			<< "decoding error when corrupting positions " << i << " to " << i+ERRORS;
	}
}

/*
 * @brief corrupt the maximally allowed amount of bytes (NPAR / 2)
 *        as a striding error.
 *
 *        So [0], [2], ..., [NPAR]. Then [1], [3], ...
 */
TEST_F(ECCTest, StrideTest) {
	uint8_t  codeword[RECVD_MSG_LENGTH];
	encode_data(message, MSG_LENGTH, codeword);

	for(size_t i = 0; i < sizeof(codeword) - NPAR; i++) {
		uint8_t copy[sizeof(codeword)];
		memcpy(copy, codeword, sizeof(copy));

		for(size_t j = 0; j < NPAR; j += 2) {
			copy[i + j] ^= 0xA5;
		}

		decode_data(copy, RECVD_MSG_LENGTH);
		EXPECT_GT(check_syndrome(), 0);
		correct_errors_erasures (copy, RECVD_MSG_LENGTH, 0, NULL);

		ASSERT_TRUE(memcmp(copy, message, sizeof(message)) == 0)
			<< "decoding error when corrupting positions " << i << " to " << i+NPAR;
	}
}

/*
 * @brief corrupt more than maximally allowed amount of bytes (NPAR / 2)
 *        as a burst error.
 */
TEST_F(ECCTest, FailBurstErrorsTest) {
	const size_t ERRORS = NPAR / 2 + 1;
	uint8_t  codeword[RECVD_MSG_LENGTH];
	encode_data(message, MSG_LENGTH, codeword);

	for(size_t i = 0; i < sizeof(codeword) - ERRORS; i++) {
		uint8_t copy[sizeof(codeword)];
		memcpy(copy, codeword, sizeof(copy));

		for(size_t j = 0; j < ERRORS; j++) {
			copy[i + j] ^= 0xA5;
		}

		decode_data(copy, RECVD_MSG_LENGTH);
		EXPECT_GT(check_syndrome(), 0);
		correct_errors_erasures (copy, RECVD_MSG_LENGTH, 0, NULL);

		//ASSERT_FALSE(memcmp(copy, message, sizeof(message)) == 0)
		//	<< "decoding error when corrupting positions " << i << " to " << i+ERRORS;
	}
}

TEST_F(ECCTest, CPPInterfaceTest) {
	auto x = std::vector<uint8_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
	auto enc = ECC::encode(x);
	auto dec = ECC::decode(enc);

	ASSERT_EQ(x, dec);
}

TEST_F(ECCTest, SizeTest) {
	/* size < MAX_MESSAGE_LENGTH is already tested by CPPInterfaceTest */

	/* two chunks */
	{
		auto x = std::vector<uint8_t>();
		for(auto i = 0; i < 300; i++) {
			x.push_back(i);
		}

		auto enc = ECC::encode(x);
		auto dec = ECC::decode(enc);

		ASSERT_EQ(x, dec);
	}

	/* three chunks */
	{
		auto x = std::vector<uint8_t>();
		for(auto i = 0; i < 600; i++) {
			x.push_back(i);
		}

		auto enc = ECC::encode(x);
		auto dec = ECC::decode(enc);

		ASSERT_EQ(x, dec);
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

