#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

#define MAX_LEN 64
typedef struct {
	int32_t offset;
	int16_t datalen;
	char data[MAX_LEN];
} PACKET;

static void _send(int s, char* buf, size_t len);
static int _connect(const char* domain, const char* service);
int main(int argc, char** argv) {
	if(argc < 2) {
		printf("%s host port\n", argv[0]);
		return 0;
	}

	int s;
	if((s = _connect(argv[1], argv[2])) < 0) {
		puts(strerror(errno));
		return -1;
	}

	char buf[MAX_LEN];
	int i = 0;
	for(char c; (c = fgetc(stdin)) != EOF; i++) {
		buf[i % sizeof(buf)] = c;
		if(i != sizeof(buf))
			continue;

		_send(s, buf, sizeof(buf));	
	}

	/* send remaining */
	_send(s, buf, (i % sizeof(buf)));	


	close(s);
	return 0;
}

static void _send(int s, char* buf, size_t len) {
	int r;
	if((r = send(s, buf, len, 0)) < len) {
		printf("send() failed: %s\n", strerror(errno));
	}
	else {
		printf("sent %d\n", r);
	}
}

/**
 * \brief Connects to the first reachable IPv4/v6 address on @service
 * \return -1 on error, else the socket 
 */ 
static int _connect(const char* domain, const char* service) {
	struct addrinfo hints = {.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_DGRAM,
		.ai_flags = 0,
		.ai_protocol = 0,
		0};
	struct addrinfo *result;
	int sock = -1;
	
	if(getaddrinfo(domain, service, &hints, &result) != 0) {
		return -1;
	}

	for(struct addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock == -1)
			continue;

		if(connect(sock, rp->ai_addr, rp->ai_addrlen) == -1) {
			close(sock);
			sock = -1;
			continue;
		}
		
		break;
	}
	freeaddrinfo(result);
	
	return sock;
}
