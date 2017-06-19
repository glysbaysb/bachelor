#include <stdio.h>
#include <string.h>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/pubsub.h>
#include <assert.h>
#include <msgpack.h>

#include "rpc.h"

int createSuscriberSocketForWorldStatus(const char* url);

int recvNanaomsg(int sock, char** buf, int* len) {
	assert(buf);
	assert(len);

	*len = nn_recv(sock, buf, NN_MSG, 0);
	return *len;
}

void parseRPCReply(char* buf, int len, struct RPCReply* reply) {
	msgpack_unpacker unp;
	msgpack_unpacker_init(&unp, 100);

	char unpacked_buffer[128];
	msgpack_unpacked result;
	msgpack_unpacked_init(&result);

	size_t off;
	int ret = msgpack_unpack_next(&result, buf, len, &off);
	while (ret == MSGPACK_UNPACK_SUCCESS) {
		msgpack_object obj = result.data;

		switch(obj.type) {
		case MSGPACK_OBJECT_ARRAY:{
			msgpack_object_array* arr = (msgpack_object_array*)&obj.via;
			if(arr->size < 4) {
				fprintf(stderr, "array too small");
				break;
			}

			reply->op = arr->ptr[0].via.i64;
			reply->id = arr->ptr[1].via.i64;
			reply->error = arr->ptr[2].via.i64;
			// todo: params
			break;
		}
		default:
			fprintf(stderr, "unhandled msgpack type:");
			msgpack_object_print(stdout, obj);
			putc('\n', stdin);
			break;
		}

		ret = msgpack_unpack_next(&result, buf, len, &off);
	}
	msgpack_unpacked_destroy(&result);

	if (ret == MSGPACK_UNPACK_CONTINUE) {
		printf("All msgpack_object in the buffer is consumed.\n");
	}
	else if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
		printf("The data in the buf is invalid format.\n");
	}

	msgpack_unpacker_destroy(&unp);
}

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("%s url\n", argv[0]);
		return 0;
	}

	int sock = nn_socket(AF_SP, NN_REQ);
	if(nn_connect(sock, argv[1]) < 0) {
		fprintf(stderr, "can't connect: %s\n", nn_strerror(nn_errno()));
		return 1;
	}

	int susSock = createSuscriberSocketForWorldStatus("tcp://localhost:8001");
	struct nn_pollfd pfd[] = {
		{.fd = sock, .events = NN_POLLIN, 0},
		{.fd = susSock, .events = NN_POLLIN, 0}
	};
	while(nn_poll(pfd, sizeof(pfd)/sizeof(pfd[0]), -1) > 0) {
		char* buf = NULL;
		int len;

		/* request - reply socket */
		if((pfd[0].revents & NN_POLLIN) == NN_POLLIN) {
			recvNanaomsg(pfd[0].fd, &buf, &len);
			pfd[0].revents = 0;
			printf("%.*s\n", len, buf);
		}
		/* publish - suscribe socket */
		else if((pfd[1].revents & NN_POLLIN) == NN_POLLIN) {
			recvNanaomsg(pfd[1].fd, &buf, &len);
			pfd[1].revents = 0;

			printf("recvd: %d\n", len);
			struct RPCReply reply;
			parseRPCReply(buf, len, &reply);

			printf("rpcreply (%d): %d\n", reply.id, reply.error);
		}

		if(buf) {
			nn_freemsg(buf);
		}
	}
	
	nn_shutdown(sock, 0);
	nn_shutdown(susSock, 0);

	return 0;
}

void synchronCall(int sock) {
	const char* msg = "hello\n";
	if(nn_send(sock, msg, strlen(msg)+1, 0) < 0) {
		fprintf(stderr, "can't send\n");
		return;
	}

	char* buf = NULL;
	int len = nn_recv(sock, &buf, NN_MSG, 0);
	if(len < 0) {
		fprintf(stderr, "can't recv\n");
		return;
	}
	printf("got %d bytes. %.*s\n", len, len, buf);
	nn_freemsg(buf);
}

int createSuscriberSocketForWorldStatus(const char* url) {
	int sock = nn_socket(AF_SP, NN_SUB);
	if(sock < 0) {
		fprintf(stderr, "can't create suscribersocket\n");
		return sock;
	}

	int s = nn_connect(sock, url);
	if(s < 0) {
		fprintf(stderr, "can't connect: %s\n", nn_strerror(nn_errno()));
		return s;
	}

	/* no filter */
	if((s = nn_setsockopt (sock, NN_SUB, NN_SUB_SUBSCRIBE, "", 0)) < 0) {
		printf("can't change socket state: %s\n", nn_strerror(nn_errno()));
		return s;
	}

	return sock;
}
