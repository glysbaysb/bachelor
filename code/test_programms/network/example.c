/* Example use of Reed-Solomon library 
 *
 * Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
 *
 * This software library is licensed under terms of the GNU GENERAL
 * PUBLIC LICENSE
 *
 * RSCODE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSCODE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Rscode.  If not, see <http://www.gnu.org/licenses/>.

 * Commercial licensing is available under a separate license, please
 * contact author for details.
 *
 * This same code demonstrates the use of the encodier and 
 * decoder/error-correction routines. 
 *
 * We are assuming we have at least four bytes of parity (NPAR >= 4).
 * 
 * This gives us the ability to correct up to two errors, or 
 * four erasures. 
 *
 * In general, with E errors, and K erasures, you will need
 * 2E + K bytes of parity to be able to correct the codeword
 * back to recover the original message data.
 *
 * You could say that each error 'consumes' two bytes of the parity,
 * whereas each erasure 'consumes' one byte.
 *
 * Thus, as demonstrated below, we can inject one error (location unknown)
 * and two erasures (with their locations specified) and the 
 * error-correction routine will be able to correct the codeword
 * back to the original message.
 * */
 
#include <stdio.h>
#include <stdlib.h>
#include <libecc/ecc.h>

#define MSG_LENGTH (255 - NPAR)
#define RECVD_MSG_LENGTH (MSG_LENGTH + NPAR)

static void _print(const unsigned char* msg, const size_t len) {
	for(size_t i = 0; i < len; i++) {
		putchar(*(msg + i));
	}
}

static void _decode(void) {
  /* Encode data into codeword, adding NPAR parity bytes */
  unsigned char codeword[RECVD_MSG_LENGTH];
  size_t i = 0;
  int c;
  while((c = getchar()) != EOF) {
    codeword[i % RECVD_MSG_LENGTH] = c;
	i++;
	if(!(i && i % RECVD_MSG_LENGTH == 0))
	  continue;

    /* Now decode -- encoded codeword size must be passed */
    decode_data(codeword, RECVD_MSG_LENGTH);
    /* check if syndrome is all zeros */
    if(check_syndrome () != 0) {
      correct_errors_erasures (codeword, 
	   	     RECVD_MSG_LENGTH,
	   	     0, 
		     NULL);
    }
	_print(codeword, MSG_LENGTH);
  }

  /* encode & print rest */
  decode_data(codeword, i % RECVD_MSG_LENGTH);
  /* check if syndrome is all zeros */
  if(check_syndrome () != 0) {
    correct_errors_erasures (codeword, 
  	     i % RECVD_MSG_LENGTH,
   	     0, 
	     NULL);
  } 
  _print(codeword, MSG_LENGTH); // mhm, lieber zu viel ausgeben als zu wenig
}

static void _encode() {
  unsigned char codeword[RECVD_MSG_LENGTH],
                message[MSG_LENGTH] = {0};
  size_t i = 0;
  int c;
  while((c = getchar()) != EOF) {
    message[i % MSG_LENGTH] = c;
	i++;
	if(!(i && i % MSG_LENGTH == 0))
		continue;

    encode_data(message, MSG_LENGTH, codeword);
    _print(codeword, RECVD_MSG_LENGTH);
  }

  /* decode & print rest */
  encode_data(message, i % MSG_LENGTH, codeword);
  _print(codeword, RECVD_MSG_LENGTH);
}

int main (int argc, char *argv[])
{
  if(argc < 2) {
    printf("%s [enc|dec]", argv[0]);
	return 0;
  }

  initialize_ecc();
  
  if(strcmp(argv[1], "enc") == 0) {
    _encode();
  } else {
    _decode();
  }

  exit(0);
}

