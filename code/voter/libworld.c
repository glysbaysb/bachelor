#include <stdio.h>
#include <string.h>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/pubsub.h>
#include <assert.h>
#include <msgpack.h>
#include <pthread.h>

#include "world.h"
#include "rpc.h"

typedef struct WorldContext_ {
	int subSock;
	int reqSock;

	void* additional;
	TypeGetWorldStatusCallback getWorldStatusCallback;
	pthread_t thread;
	int stopThread;
} WorldContext;

static int recvNanaomsg(int sock, char** buf, int* len) {
	assert(buf);
	assert(len);

	*len = nn_recv(sock, buf, NN_MSG, 0);
	return *len;
}

static void parseWorldStatus(char* buf, size_t len) {
	msgpack_unpacked result;
	msgpack_unpacked_init(&result);

	size_t off = 0;
	int cont = 1;//used to break out of the while in case of an error
	int ret = msgpack_unpack_next(&result, buf, len, &off);
	while (ret == MSGPACK_UNPACK_SUCCESS && cont) {
		msgpack_object obj = result.data;

		switch(obj.type) {
		case MSGPACK_OBJECT_ARRAY:{
			msgpack_object_array* arr = (msgpack_object_array*)&obj.via;
			WorldStatus ws = {.numOfObjects = arr->size,
				.objects = calloc(arr->size, sizeof(SimulationObject),
				0};
			if(!ws.objects) {
				cont = 0;
				break;
			}
			
			for(size_t i = 0; i < arr->size; i++) {
				printf("arr elem %zu is a %s\n", i, typeToStr[arr->ptr[i].type]);
			}
		}
		break;
		default:
		break;
		}

		ret = msgpack_unpack_next(&result, buf, len, &off);
	}
	msgpack_unpacked_destroy(&result);

	if (ret == MSGPACK_UNPACK_CONTINUE) {
		printf("ws:All msgpack_object in the buffer is consumed.\n");
	}
	else if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
		printf("ws:The data in the buf is invalid format.\n");
	}
}

static void parseRPCReply(char* buf, size_t len, struct RPCReply* reply) {
	msgpack_unpacked result;
	msgpack_unpacked_init(&result);

	const char* typeToStr[] = {"nil", "boolean", "pos int", "neg int",
					"float", "str", "array", "map", "bin", "ext"};
	size_t off = 0;
	int ret = msgpack_unpack_next(&result, buf, len, &off);
	while (ret == MSGPACK_UNPACK_SUCCESS) {
		msgpack_object obj = result.data;

		switch(obj.type) {
		case MSGPACK_OBJECT_ARRAY:{
			msgpack_object_array* arr = (msgpack_object_array*)&obj.via;
			if(arr->size < 4) {
				fprintf(stdout, "array too small");
				break;
			}

			for(size_t i = 0; i < arr->size; i++) {
				printf("arr elem %zu is a %s\n", i, typeToStr[arr->ptr[i].type]);
			}
			reply->op = arr->ptr[0].via.i64;
			reply->id = arr->ptr[1].via.i64;
			reply->error = arr->ptr[2].via.i64;
			// todo: params
			break;
		}
		default:
			fprintf(stdout, "unhandled msgpack type:");
			msgpack_object_print(stdout, obj);
			putchar('\n');
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
}

static char* synchronCall(int sock, const char* msg, const size_t lenIn, int* lenOut) {
	assert(lenOut);

	if(nn_send(sock, msg, lenIn, 0) < 0) {
		fprintf(stderr, "can't send\n");
		return NULL;
	}

	char* buf = NULL;
	*lenOut = nn_recv(sock, &buf, NN_MSG, 0);
	if(*lenOut <= 0) {
		fprintf(stderr, "can't recv\n");
		return NULL;
	}

	return buf;
}

static int createSuscriberSocketForWorldStatus(const char* url) {
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

void* connectToWorld(void) {
	WorldContext* wc = NULL;

	if(!(wc = calloc(1, sizeof(WorldContext)))) {
		fprintf(stderr, "can't allocate WorldContext\n");
		return NULL;
	}

	wc->reqSock = nn_socket(AF_SP, NN_REQ);
	if(nn_connect(wc->reqSock, "tcp://localhost:8000") < 0) {
		fprintf(stderr, "can't connect: %s\n", nn_strerror(nn_errno()));

		free(wc);

		return NULL;
	}

	wc->subSock = createSuscriberSocketForWorldStatus("tcp://localhost:8001");

	return wc;
}

void detachFromWorld(void* ctx_) {
	WorldContext* ctx = (WorldContext*)ctx_;

	/* tell the other thread to exit and then wait for completion */
	ctx->stopThread = 1;
	if(pthread_join(ctx->thread, NULL) < 0) {
		fprintf(stderr, "can't join thread\n");
	}

	nn_shutdown(ctx->reqSock, 0);
	nn_shutdown(ctx->subSock, 0);

	free(ctx_);
}

static void* networkHandler(void* ctx_) {
	WorldContext* ctx = (WorldContext*)ctx_;

	struct nn_pollfd pfd[] = {
		{.fd = ctx->reqSock, .events = NN_POLLIN, 0},
		{.fd = ctx->subSock, .events = NN_POLLIN, 0}
	};
	while(nn_poll(pfd, sizeof(pfd)/sizeof(pfd[0]), -1) > 0 && ctx->stopThread == 0) {
		char* buf = NULL;
		int len;

		/* request - reply socket */
		if((pfd[0].revents & NN_POLLIN) == NN_POLLIN) {
			recvNanaomsg(pfd[0].fd, &buf, &len);
			pfd[0].revents = 0;

			printf("recvd: %d\n", len);
			struct RPCReply reply;
			parseRPCReply(buf, len, &reply);

			printf("rpcreply (%d): %d\n", reply.id, reply.error);
		}
		/* publish - suscribe socket */
		else if((pfd[1].revents & NN_POLLIN) == NN_POLLIN) {
			recvNanaomsg(pfd[1].fd, &buf, &len);
			pfd[1].revents = 0;

			printf("got a published event: ");
			for(size_t i = 0; i < len; i++) {
				printf("%02X ", (buf[i] & 0xFF));
			}
			putchar('\n');
			parseWorldStatus(buf, len);

		}

		if(buf) {
			nn_freemsg(buf);
		}
	}
	

	return NULL;
}


int startProcessingWorldEvents(void* ctx_, TypeGetWorldStatusCallback cb, void* additional) {
	WorldContext* ctx = (WorldContext*)ctx_;

	ctx->getWorldStatusCallback = cb;
	ctx->additional = additional;

	pthread_attr_t attr;
	int s = pthread_attr_init(&attr);
	if (s != 0) {
		return -1;
	}

	if(pthread_create(&ctx->thread, &attr, &networkHandler, ctx_) < 0) {
		pthread_attr_destroy(&attr);
		return -2;
	}

	pthread_attr_destroy(&attr);

	return 0;
}

void MoveRobot(void* ctx_, int id, int diffX, int diffY) {
	(void)id; (void)diffX; (void) diffY;

	WorldContext* ctx = (WorldContext*)ctx_;
	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	//msgpack_pack_array(&pk, 4);
	msgpack_pack_int32(&pk, REQUEST); // operation
	msgpack_pack_int32(&pk, 0x1234ABCD); // id
	msgpack_pack_int8(&pk, 0x1A); // procedure
	msgpack_pack_array(&pk, 3);

	{
		msgpack_pack_int32(&pk, id);
		msgpack_pack_int32(&pk, diffX);
		msgpack_pack_int32(&pk, diffY);
	}

	for(size_t i = 0; i < sbuf.size; i++) {
		printf("%02X ", (sbuf.data[i] & 0xFF));
	}
	putchar('\n');

	if(nn_send(ctx->reqSock, sbuf.data, sbuf.size, 0) < 0) {
		fprintf(stderr, "can't send MoveRobot rpc request\n");
	}
	printf("moverobot finished\n");
}
