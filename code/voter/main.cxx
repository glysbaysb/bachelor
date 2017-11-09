#include <iostream>
#include <iterator>
#include <cstdio>
#include <unistd.h>
#include <libworld/world.h>
#include <libnetwork/network.h>

#include "voter.h"

typedef struct {
	int robot;
	void* worldCtx;
	Network* network;
} Info;

static int sendWorldStatus(const WorldStatus* ws, Network* network);
static void voteCallback(void* optional, msgpack_object_array* params);
static void worldStatusRPCCallback(void* optional, msgpack_object_array* params);
static void worldStatusCallback(const WorldStatus* ws, void* additional);

int main(int argc, char** argv)
{
	Info callbackInfo = {0};

	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}

	callbackInfo.network = new Network(argv[2]);
	callbackInfo.network->addRPCHandler(Procedure::VOTE_MOVE_ROBOT, &voteCallback, (void*)&callbackInfo);
	callbackInfo.network->addRPCHandler(Procedure::WORLD_STATUS, &worldStatusRPCCallback, (void*)&callbackInfo);

	if(!(callbackInfo.worldCtx = connectToWorld(argv[1]))) {
		fprintf(stderr, "can't init world");
		return 1;
	}

	callbackInfo.robot = createRobot(callbackInfo.worldCtx);
	printf("Created robot %d\n", callbackInfo.robot);
	startProcessingWorldEvents(callbackInfo.worldCtx, &worldStatusCallback, (void*)&callbackInfo);

	while(1) {
		sleep(1);
	}

	detachFromWorld(callbackInfo.worldCtx);
}

static int sendWorldStatus(const WorldStatus* ws, Network* network)
{
	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 3);
	msgpack_pack_float(&pk, ws->xTilt);
	msgpack_pack_float(&pk, ws->yTilt);

	msgpack_pack_array(&pk, ws->numOfObjects);
	for(auto i = 0; i < ws->numOfObjects; i++) {
		msgpack_pack_array(&pk, 6);

		msgpack_pack_int32(&pk, ws->objects[i].id);
		msgpack_pack_int32(&pk, ws->objects[i].type);

		msgpack_pack_float(&pk, ws->objects[i].x);
		msgpack_pack_float(&pk, ws->objects[i].y);

		msgpack_pack_float(&pk, ws->objects[i].m);
		msgpack_pack_float(&pk, ws->objects[i].fuel);
	}

	if(network->sendRPC(Procedure::WORLD_STATUS, sbuf.data, sbuf.size) < 0) {
		return -1;
	}

	return 0;
}

static void worldStatusCallback(const WorldStatus* ws, void* additional)
{
	Info* info = (Info*)additional;

	printf("(%f:%f)\n", ws->xTilt, ws->yTilt);

	for(size_t i = 0; i < ws->numOfObjects; i++) {
		printf("#%d is a %s\n", ws->objects[i].id, ws->objects[i].type == ROBOT ? "robot" : "fuel station");
		printf("\tPos: (%f:%f)\n", ws->objects[i].x, ws->objects[i].y);
		printf("\tMass: %f\n", ws->objects[i].m);

		if(ws->objects[i].type == ROBOT && info->robot == ws->objects[i].id) {
			printf("\tFuel: %d\n", ws->objects[i].fuel);

			int r = 0;
			if((r = moveRobot(info->worldCtx, ws->objects[i].id, 1, 45)) < 0) {
				fprintf(stderr, "can't move robot: %d", r);
			}
		}
	}

	if(sendWorldStatus(ws, info->network) < 0) {
		fprintf(stderr, "can't send world status\n");
		return;
	}
}

static void voteCallback(void* optional, msgpack_object_array* params)
{
    if(!params || !params->size) {
        return;
    }

    for(size_t i = 0; i < params->size; i++) {
        switch(params->ptr[i].type) {
        case MSGPACK_OBJECT_ARRAY:
            voteCallback(optional, (msgpack_object_array*) &params->ptr[i].via);
            break;

        case MSGPACK_OBJECT_FLOAT32:
        case MSGPACK_OBJECT_FLOAT64:
			std::cout << params->ptr[i].via.f64;
            break;

        case MSGPACK_OBJECT_NEGATIVE_INTEGER:
			std::cout << params->ptr[i].via.u64;
            break;

        case MSGPACK_OBJECT_POSITIVE_INTEGER:
			std::cout << params->ptr[i].via.i64;
            break;

        case MSGPACK_OBJECT_STR:
			auto str = ((const msgpack_object_str*)&params->ptr[i].via.str);
			std::copy(str->ptr, str->ptr + str->size, std::ostream_iterator<char>(std::cout));
            break;
        }
    }
}

static void worldStatusRPCCallback(void* optional, msgpack_object_array* params)
{
}

