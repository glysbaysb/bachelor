// https://github.com/jgaeddert/liquid-dsp
// fec_example.c
//
// This example demonstrates the interface for forward error-
// correction (FEC) codes.  A buffer of data bytes is encoded and
// corrupted with several errors.  The decoder then attempts to
// recover the original data set.  The user may select the FEC
// scheme from the command-line interface.
// SEE ALSO: crc_example.c
//           checksum_example.c
//           packetizer_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <liquid/liquid.h>

// print usage/help message
void usage()
{
    printf("fec_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  d     : decode\n");
    printf("  c     : coding scheme, (h128 default):\n");
    liquid_print_fec_schemes();
}


int main(int argc, char*argv[])
{
    fec_scheme fs = LIQUID_FEC_HAMMING128;   // error-correcting scheme
	int decode = 0;

    int dopt;
    while((dopt = getopt(argc,argv,"uhdc:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'c':
            fs = liquid_getopt_str2fec(optarg);
            if (fs == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported fec scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
		case 'd':
			decode = 1;
			break;
        default:
            exit(1);
        }
    }

    // ensure proper data length
    const unsigned int n = 48; // Vielfaches von 8 und 12

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char data[n_enc];          // original data message / recvd message

    // create object
    fec q = fec_create(fs,NULL);
    //fec_print(q);

    unsigned int i = 0;
	int c;
	const unsigned int bufsize = (decode == 0 ? n : n_enc);
	while((c = getchar()) != EOF) {
		data[i % bufsize] = c;
		i++;
		if(!(i && i % bufsize == 0))
			continue;

		// encode message
		if(decode == 0) {
			unsigned char msg_enc[n_enc];   // encoded data message
			fec_encode(q, n, data, msg_enc);
			for (int j=0; j<n_enc; j++) {
				printf("%c", (unsigned char) (msg_enc[j]));
			}
		} else {
			unsigned char msg_dec[n];       // decoded data message
			memset(msg_dec, 0, n);
			fec_decode(q, n, data, msg_dec);
			for (int j=0; j<n; j++)
				printf("%c", (unsigned char) (msg_dec[j]));
		}
	}

#if 0
	/* handle remaining */
	if(decode == 0) {
		fec_encode(q, n, data, msg_enc);
		for (i=0; i<n_enc; i++) {
			printf("%c", (unsigned char) (msg_enc[i]));
		}
	} else {
		fec_decode(q, n, data, msg_dec);
		for (i=0; i<n; i++)
			printf("%c", (unsigned char) (msg_dec[i]));
	}
#endif

    // clean up objects
    fec_destroy(q);

    return 0;
}

