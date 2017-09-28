//
// Created by a on 9/24/17.
//

#ifndef FT_ROBOT_SIMULATION_ECC_H
#define FT_ROBOT_SIMULATION_ECC_H
#ifdef __cplusplus
extern "C" {
#endif

/* Old C interface */
#include "_ecc.h"
void initialize_ecc (void);
int check_syndrome (void);
void decode_data (unsigned char data[], int nbytes);
void encode_data (const unsigned char msg[], int nbytes, unsigned char dst[]);
int correct_errors_erasures (unsigned char codeword[], int csize,int nerasures, int erasures[]);
#define MAX_MESSAGE_LENGTH (255 - NPAR)
#define OUTPUT_LENGTH 255
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
/**
 *
 */
namespace ECC
{
	// todo: what happens if this is called multiple times?
	void initialize()
	{
		initialize_ecc();
	}

	std::vector<uint8_t> encode(const std::vector<uint8_t>& in)
	{
		assert(in.size() <= MAX_MESSAGE_LENGTH);
		std::vector<uint8_t> out;

		out.reserve(OUTPUT_LENGTH);
		out.resize(in.size() + NPAR);

		encode_data(in.data(), in.size(), out.data());
		return out;
	}

	std::vector<uint8_t> decode(const std::vector<uint8_t>& in)
	{
		auto decoded = std::vector<uint8_t>(in);
		decode_data(decoded.data(), decoded.size());

		if(check_syndrome () != 0) {
			correct_errors_erasures (decoded.data(),
				 in.size(),
				 0,
				 NULL);
		}

		decoded.resize(in.size() - NPAR);
		return decoded;
	}
}
#endif
#endif //FT_ROBOT_SIMULATION_ECC_H
