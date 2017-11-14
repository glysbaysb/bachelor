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
#define MAX_MESSAGE_LENGTH (255u - NPAR)
#define OUTPUT_LENGTH 255u
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
		auto chunks = (in.size() / MAX_MESSAGE_LENGTH) + 1;

		std::vector<uint8_t> out;
		out.reserve(in.size() + (NPAR * chunks));
		out.resize(in.size() + (NPAR * chunks));

		for(auto i = 0; i < chunks; i++) {
			auto left = in.size() - (i * MAX_MESSAGE_LENGTH);

			encode_data(in.data() + (MAX_MESSAGE_LENGTH * i), 
					std::min<size_t>(left, MAX_MESSAGE_LENGTH),
					out.data() + (OUTPUT_LENGTH * i));
		}

		return out;
	}

	std::vector<uint8_t> decode(const std::vector<uint8_t>& in)
	{
		assert(in.size() > NPAR);

		auto decoded = std::vector<uint8_t>();

		auto chunks = (in.size() / OUTPUT_LENGTH) + 1;
		for(auto i = 0; i < chunks; i++) {
			auto chunk = std::vector<uint8_t>(in.data() + (OUTPUT_LENGTH * i),
					std::min(in.data() + (OUTPUT_LENGTH * (i + 1)), in.data() + in.size()));
			decode_data(chunk.data(), chunk.size());

			if(check_syndrome () != 0) {
				correct_errors_erasures (chunk.data(),
					 chunk.size(),
					 0,
					 NULL);
			}

			chunk.resize(chunk.size() - NPAR);
			decoded.insert(decoded.end(), chunk.begin(), chunk.end());
		}

		return decoded;
	}
}
#endif
#endif //FT_ROBOT_SIMULATION_ECC_H
