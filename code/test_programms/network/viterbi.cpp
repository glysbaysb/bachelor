// https://github.com/Venemo/fecmagic.git
// This file is part of fecmagic, the forward error correction library.
// Copyright (c) 2016 Timur Kristóf
// Licensed to you under the terms of the MIT license.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <random>
#include "fecmagic/src/binaryprint.h"
#include "fecmagic/src/convolutional-encoder.h"
#include "fecmagic/src/convolutional-decoder.h"

//#define TEST_CONVOLUTIONAL_DECODER_DEBUG
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
#   define DEBUG_PRINT(x) (::std::cout << "[TestConvolutionalDecoder] " << x << std::endl);
#else
#   define DEBUG_PRINT(x)
#endif

using namespace std;
using namespace fecmagic;

constexpr uint8_t poly1 = 0x5b;
constexpr uint8_t poly2 = 0x79;

static void _print(const unsigned char* msg, const size_t len) {
	for(size_t i = 0; i < len; i++) {
		putchar(*(msg + i));
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("%s [enc|dec]\n", argv[0]);
		return 0;
	}

	/* read all data */
	std::vector<uint8_t> buf;
	int c;
	while((c = getchar()) != EOF) {
		buf.push_back(c);
	}

	/* en-/decode it */
	if(strcmp(argv[1], "enc") == 0) {
		ConvolutionalEncoder<3, uint8_t, 7, 5> enc;
		size_t encodedSize = enc.calculateOutputSize(buf.size());
		uint8_t *encOutput = new uint8_t[encodedSize];

		enc.reset(encOutput);
		enc.encode(buf.data(), buf.size());
		enc.flush();

		_print(encOutput, encodedSize);

		delete [] encOutput;
	} else {
		ConvolutionalDecoder<5, 3, uint8_t, 7, 5> dec;
		size_t decodedSize = dec.calculateOutputSize(buf.size());
		uint8_t *decOutput = new uint8_t[decodedSize];

		dec.reset(decOutput);
		dec.decode(buf.data(), buf.size());
		dec.flush();
	 
		_print(decOutput, decodedSize);

		delete [] decOutput;
	}
    
    return 0;
}

