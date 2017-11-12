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
		callbackInfo.network->poll(1);
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
		msgpack_pack_array(&pk, 7);

		msgpack_pack_int32(&pk, ws->objects[i].id);
		msgpack_pack_int32(&pk, ws->objects[i].type == ROBOT ? 'R' : 'F');

		msgpack_pack_float(&pk, ws->objects[i].x);
		msgpack_pack_float(&pk, ws->objects[i].y);
		msgpack_pack_float(&pk, ws->objects[i].rotation);

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
		}
	}

	if(sendWorldStatus(ws, info->network) < 0) {
		fprintf(stderr, "can't send world status\n");
		return;
	}
}

/**
 * todo: well vote
 */
static void voteCallback(void* optional, msgpack_object_array* params)
{
	Info* info = (Info*)optional;

	assert(params->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[0].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	auto id = params->ptr[0].via.i64;

	assert(params->ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[1].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	auto x = params->ptr[1].via.i64;
	// todo: clamp

	assert(params->ptr[2].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[2].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	auto y = params->ptr[2].via.i64;
	// todo: clamp

	std::cout << "Move " << id << ' ' << x << ';' << y << '\n';
	int r = 0;
	if((r = moveRobot(info->worldCtx, id, x, y)) < 0) {
		fprintf(stderr, "can't move robot: %d", r);
	}
}

static void worldStatusRPCCallback(void* optional, msgpack_object_array* params)
{
	(void) optional; (void) params;
}

