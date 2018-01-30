#include <stdio.h>
#include <string.h>
#include <nanomsg/src/nn.h>
#include <nanomsg/src/reqrep.h>
#include <nanomsg/src/pubsub.h>
#include <assert.h>
#include <msgpack.h>
#include <pthread.h>
#include <unistd.h>

#include "world.h"
#include <librpc/rpc.h>
#include <libficfg/ficfg.h>

typedef struct WorldContext_ {
	void* rpc;

	int subSock;
	int reqSock;

	void* additional;
	TypeGetWorldStatusCallback getWorldStatusCallback;
	pthread_t thread;
	int stopThread;

	struct {
		int id;
		// todo: mutex
	} createRobot;

	struct FiCfg cfg;
	WorldStatus* ws; //! saved between invocations for dupWorldStatus
} WorldContext;

static void createRobotCallback(void* optional, msgpack_object_array* params);
static void moveRobotCallback(void* optional, msgpack_object_array* params);
static int getFiCfg(void* ctx_);

static int recvNanaomsg(int sock, char** buf, int* len) {
	assert(buf);
	assert(len);

	*len = nn_recv(sock, buf, NN_MSG, 0);
	return *len;
}

static int parseObject(SimulationObject* object, const msgpack_object_array* arr) {
	if(arr->size < 6)
		return -1;

	/* reply is: [1000, -471718, 31.000000, 0.605107, "ROBOT", -0.044753, -14.968350] */

	assert(arr->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER || arr->ptr[0].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	object->fuel = arr->ptr[0].via.i64;

	assert(arr->ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER || arr->ptr[1].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	object->id = arr->ptr[1].via.i64;

	assert(arr->ptr[2].type == MSGPACK_OBJECT_FLOAT || arr->ptr[2].type == MSGPACK_OBJECT_FLOAT32);
	object->m = arr->ptr[2].via.f64;

	assert(arr->ptr[4].type == MSGPACK_OBJECT_STR);
	object->type = arr->ptr[4].via.str.ptr[0] == 'R' ? ROBOT : FUEL_STATION; // todo, is a string

	assert(arr->ptr[5].type == MSGPACK_OBJECT_FLOAT || arr->ptr[5].type == MSGPACK_OBJECT_FLOAT32);
	object->x = arr->ptr[5].via.f64;
	assert(arr->ptr[6].type == MSGPACK_OBJECT_FLOAT || arr->ptr[6].type == MSGPACK_OBJECT_FLOAT32);
	object->y = arr->ptr[6].via.f64;

	assert(arr->ptr[3].type == MSGPACK_OBJECT_FLOAT || arr->ptr[3].type == MSGPACK_OBJECT_FLOAT32);
	object->rotation = arr->ptr[3].via.f64;

	return 0;
}

static WorldStatus* parseWorldStatus(char* buf, size_t len) {
	WorldStatus* ws = NULL;

	msgpack_unpacked result;
	msgpack_unpacked_init(&result);

	size_t off = 0;
	int cont = 1;//used to break out of the while in case of an error
	int ret = msgpack_unpack_next(&result, buf, len, &off);
	while (ret == MSGPACK_UNPACK_SUCCESS && cont) {
		msgpack_object obj = result.data;

#if 0
		for(size_t i = 0; i < len; i++) {
			printf("%02X ", (buf[i] & 0xFF));
		}
		putchar('\n');
		msgpack_object_print(stdout, obj);
		putchar('\n');
#endif

		switch(obj.type) {
		case MSGPACK_OBJECT_ARRAY:{
			msgpack_object_array* arr = (msgpack_object_array*)&obj.via;
			if(arr->ptr[0].type != MSGPACK_OBJECT_ARRAY ||
				arr->ptr[1].type != MSGPACK_OBJECT_ARRAY)
			{
				continue;
			}

			msgpack_object_array* objectsArr = (msgpack_object_array*)&arr->ptr[1].via;
			ws = (WorldStatus*)calloc(1, sizeof(WorldStatus));
			ws->numOfObjects = objectsArr->size;
			if(!(ws->objects = calloc(objectsArr->size, sizeof(SimulationObject)))) {
				cont = 0;
				break;
			}
			
			/* parse world tilt degree */
			if(arr->ptr[0].type != MSGPACK_OBJECT_ARRAY) {
				cont = 0;
				break;
			}
			ws->xTilt = ((msgpack_object_array*)&arr->ptr[0].via)->ptr[0].via.f64;
			ws->yTilt = ((msgpack_object_array*)&arr->ptr[0].via)->ptr[1].via.f64;


			/* parse objects */
			for(size_t i = 0; i < objectsArr->size; i++) {
				assert(objectsArr->ptr[i].type == MSGPACK_OBJECT_ARRAY);

				if(parseObject(&ws->objects[i], (msgpack_object_array*)&objectsArr->ptr[i].via) < 0)
					fprintf(stderr, "arr elem %zu couldn't be parsed as Object\n", i);
			}
			cont = 0;
		}
		break;
		default:
		break;
		}

		ret = msgpack_unpack_next(&result, buf, len, &off);
	}
	msgpack_unpacked_destroy(&result);

	return ws;
}

static char* synchronCall(int sock, const char* msg, const size_t lenIn, int* lenOut) {
	assert(lenOut);

	if(nn_send(sock, msg, lenIn, 0) < 0) {
		fprintf(stderr, "can't send %s\n", nn_strerror(nn_errno()));
		return NULL;
	}

	char* buf = NULL;
	*lenOut = nn_recv(sock, &buf, NN_MSG, 0);
	if(*lenOut <= 0) {
		fprintf(stderr, "can't recv\n");
		return NULL;
	}

#if 0
	for(size_t i = 0; i < *lenOut; i++) {
		printf("%02X ", (buf[i] & 0xFF));
	}
	putchar('\n');
#endif

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

static int initalizeRPC(WorldContext* wc) {
	if(!(wc->rpc = createRPCContext())) {
		return -1;
	}

	if(addProcedure(wc->rpc, CREATE_ROBOT, &createRobotCallback, wc) < 0) {
		return -2;
	}

	if(addProcedure(wc->rpc, MOVE_ROBOT, &moveRobotCallback, wc) < 0) {
		return -3;
	}

	if(addProcedure(wc->rpc, GET_FI_CFG, &getFiCfgCallback, &wc->cfg) < 0) {
		return -4;
	}


	return 0;
}

void* connectToWorld(const char* host) {
	char reqSockHost[128] = {0},
		 subSockHost[128] = {0};
	WorldContext* wc = NULL;

	if(snprintf(reqSockHost, sizeof(reqSockHost), "tcp://%s:8000", host) < 0 ||
		snprintf(subSockHost, sizeof(subSockHost), "tcp://%s:8001", host) < 0)
	{
		fprintf(stderr, "can't create host strings\n");
		return NULL;
	}

	if(!(wc = calloc(1, sizeof(WorldContext)))) {
		fprintf(stderr, "can't allocate WorldContext\n");
		return NULL;
	}

	wc->reqSock = nn_socket(AF_SP, NN_REQ);
	if(nn_connect(wc->reqSock, reqSockHost) < 0) {
		fprintf(stderr, "can't connect: %s\n", nn_strerror(nn_errno()));

		free(wc);

		return NULL;
	}

	if((wc->subSock = createSuscriberSocketForWorldStatus(subSockHost)) < 0) {
		fprintf(stderr, "can't connect: %s\n", nn_strerror(nn_errno()));

		nn_close(wc->reqSock);
		free(wc);

		return NULL;
	}

	if(initalizeRPC(wc) < 0) {
		fprintf(stderr, "can't init rpc\n");

		nn_shutdown(wc->reqSock, 0);
		nn_shutdown(wc->subSock, 0);
		free(wc);
		return NULL;
	}

	getFiCfg(wc);

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
	int r = 0;
	while(( r = nn_poll(pfd, sizeof(pfd)/sizeof(pfd[0]), -1)) > 0 && ctx->stopThread == 0) {
		unsigned char* buf = NULL;
		size_t len;

		/* request - reply socket */
		if((pfd[0].revents & NN_POLLIN) == NN_POLLIN) {
			recvNanaomsg(pfd[0].fd, &buf, &len);
			assert(len > 0);
			pfd[0].revents = 0;

#if 0
			for(size_t i = 0; i < len; i++) {
				printf("%02X ", (buf[i] & 0xFF));
			}
			putchar('\n');
#endif

			handleRPC(ctx->rpc, buf, len);
		}
		/* publish - suscribe socket */
		else if((pfd[1].revents & NN_POLLIN) == NN_POLLIN) {
			recvNanaomsg(pfd[1].fd, &buf, &len);
			assert(len > 0);
			pfd[1].revents = 0;

			WorldStatus* new = parseWorldStatus(buf, len);
			assert(new);

			/* fault injector */
			if(FAULT(ctx->cfg.dropWorldStatus)) {
				puts("dropped\n");
				/* intentionally left empty */
			} else if(FAULT(ctx->cfg.fakeWorldStatus)) {
				puts("faked\n");
				fakeWorldStatus(new);
				ctx->getWorldStatusCallback(new, ctx->additional);
			} else if(FAULT(ctx->cfg.dupWorldStatus)) {
				puts("dup\n");
				ctx->getWorldStatusCallback(ctx->ws, ctx->additional);
			} else {
				ctx->getWorldStatusCallback(new, ctx->additional);
			}

			if(ctx->ws) {
				free(ctx->ws);
			}
			ctx->ws = new;
		}

		if(buf) {
			nn_freemsg(buf);
		}
	}
	if(r < 0) {
		fprintf(stderr, "%s", nn_strerror(nn_errno()));
		return NULL;
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
		fprintf(stderr, "can't pthread_attr_init: %d\n", errno);
		return -1;
	}

	if(pthread_create(&ctx->thread, &attr, &networkHandler, ctx_) < 0) {
		fprintf(stderr, "can't pthread_create: %d\n", errno);
		pthread_attr_destroy(&attr);
		return -2;
	}

	pthread_attr_destroy(&attr);

	return 0;
}

static float clamp(float v, float min, float max)
{
	assert(max > min);

	if(v < min) {
		v = min;
	} else if(v > max) {
		v = max;
	}

	return v;
}

int moveRobot(void* ctx_, int id, float vL, float vR) {
	WorldContext* ctx = (WorldContext*)ctx_;

	/* is this request for a robot that this voter is allowed to control? */
	if(ctx->createRobot.id != id) {
		return -3;
	}

	/* create params */
	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 3);
	msgpack_pack_int32(&pk, id);
	msgpack_pack_int32(&pk, clamp(vL, -100., 100.));
	msgpack_pack_int32(&pk, clamp(vR, -100., 100.));

	/* create request with params */
	void* out = NULL; size_t outLen = 0;
	if((createRPCRequest(ctx->rpc, MOVE_ROBOT, sbuf.data, sbuf.size, &out, &outLen) < 0)) {
		return -1;
	}

	if(nn_send(ctx->reqSock, out, outLen, 0) < 0) {
		fprintf(stderr, "can't send MoveRobot rpc request\n");
		free(out);
		return -2;
	}
	free(out);

	return 0;
}

static void moveRobotCallback(void* optional, msgpack_object_array* params) {
	(void) optional; (void) params;
}

static void createRobotCallback(void* optional, msgpack_object_array* params) {
	WorldContext* ctx = (WorldContext*)optional;

	assert(params->size == 1);
	assert(params->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[0].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	ctx->createRobot.id = params->ptr[0].via.i64;
}

int createRobot(void* ctx_) {
	WorldContext* ctx = (WorldContext*)ctx_;
	
	void* out = NULL;
	size_t outLen = 0;
	if((createRPCRequest(ctx->rpc, CREATE_ROBOT, NULL, 0, &out, &outLen) < 0)) {
		return -1;
	}

	printf("rpc request size: %zu at %p\n", outLen, out);

	int lenOut;
	unsigned char* reply = synchronCall(ctx->reqSock, out, outLen, &lenOut);
	free(out);
	if(!reply) {
		return -2;
	}

	handleRPC(ctx->rpc, reply, lenOut);

	// todo: replace with mutex
	while(ctx->createRobot.id == 0) {
		usleep(100);
	}

	nn_freemsg(reply);

	return ctx->createRobot.id;
}

static int getFiCfg(void* ctx_) {
	WorldContext* ctx = (WorldContext*)ctx_;

	void* out = NULL;
	size_t outLen = 0;
	if((createRPCRequest(ctx->rpc, GET_FI_CFG, NULL, 0, &out, &outLen) < 0)) {
		return -1;
	}

	int lenOut;
	unsigned char* reply = synchronCall(ctx->reqSock, out, outLen, &lenOut);
	free(out);
	if(!reply) {
		return -2;
	}

	handleRPC(ctx->rpc, reply, lenOut);

	nn_freemsg(reply);
}
