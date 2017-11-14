#include <iostream>
#include <math.h>
#include <memory>

#include <libnetwork/network.h>
#include <libalgo/algo.h>
#include "structs.h"

static WorldStatus unpackWorldStatus(msgpack_object_array* params);
static void voteCallback(void* optional, msgpack_object_array* params);
static int sendVote(Network* network, const Action& a);

static void worldStatusCallback(void* optional, msgpack_object_array* params)
{
	Network* network = (Network*)optional;

	auto ws = unpackWorldStatus(params);
	std::cout << ws << '\n';

	/* algo */
	auto actions = calc_movement({ws.xTilt_, ws.yTilt_}, ws.robots, *ws.fuelStation);

	/* weg senden */
	for(auto&& i : actions) {
		if(sendVote(network, i) < 0) {
			std::cerr << "can't send " << i << '\n';
		}
	}
}

int main(int argc, char** argv)
{
	if(argc < 2) {
		printf("%s interface\n", argv[0]);
		return 0;
	}

	auto network = Network(argv[1]);
	network.addRPCHandler(Procedure::WORLD_STATUS, &worldStatusCallback, &network);
	network.addRPCHandler(Procedure::VOTE_MOVE_ROBOT, &voteCallback, (void*)&network);

	while(1)
	{
		network.poll(1);
	}

	return 0;
}

static WorldStatus unpackWorldStatus(msgpack_object_array* params)
{
	WorldStatus ws = {0};

	/**
	 * WorldStatus is
	 * - Angle (float, float)
	 * - Array of Object
	 */
	assert(params->ptr[0].type == MSGPACK_OBJECT_FLOAT || params->ptr[0].type == MSGPACK_OBJECT_FLOAT32);
	ws.xTilt_ = params->ptr[0].via.f64;

	assert(params->ptr[1].type == MSGPACK_OBJECT_FLOAT || params->ptr[1].type == MSGPACK_OBJECT_FLOAT32);
	ws.yTilt_ = params->ptr[1].via.f64;

	assert(params->ptr[2].type == MSGPACK_OBJECT_ARRAY);
	auto objectArr = (msgpack_object_array*)&params->ptr[2].via;
	for(auto i = 0u; i < objectArr->size; i++) {
		assert(objectArr->ptr[i].type == MSGPACK_OBJECT_ARRAY);
		auto object = (msgpack_object_array*)&objectArr->ptr[i].via;

		/* Object is:
		* - ID (int)
		* - type (int)
		* - x, y (float)
		* - rotation (float)
		* - m (float)
		* - fuel (int)
		*/
		assert(object->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER || object->ptr[0].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
		auto ID = object->ptr[0].via.i64;

		assert(object->ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER || object->ptr[1].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
		auto type = object->ptr[1].via.i64;

		assert(object->ptr[2].type == MSGPACK_OBJECT_FLOAT || object->ptr[2].type == MSGPACK_OBJECT_FLOAT32);
		auto x = object->ptr[2].via.f64;
		assert(object->ptr[3].type == MSGPACK_OBJECT_FLOAT || object->ptr[3].type == MSGPACK_OBJECT_FLOAT32);
		auto y = object->ptr[3].via.f64;

		assert(object->ptr[4].type == MSGPACK_OBJECT_FLOAT || object->ptr[4].type == MSGPACK_OBJECT_FLOAT32);
		auto rotation = object->ptr[4].via.f64;

		assert(object->ptr[5].type == MSGPACK_OBJECT_FLOAT || object->ptr[5].type == MSGPACK_OBJECT_FLOAT32);
		auto m = object->ptr[5].via.f64;

		assert(object->ptr[6].type == MSGPACK_OBJECT_FLOAT || object->ptr[6].type == MSGPACK_OBJECT_FLOAT32);
		auto fuel = object->ptr[6].via.f64;

		if(type == 'R') {
			ws.robots.emplace_back(Robot(ID, {x, y}, rotation, fuel));
		} else {
			ws.fuelStation = new FuelStation(ID, Vector(x, y), rotation);
		}
	}

	return ws;
}


static void voteCallback(void* optional, msgpack_object_array* params)
{
	(void) optional; (void) params;
}

static int sendVote(Network* network, const Action& a)
{
	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 3);
	msgpack_pack_int(&pk, a.id());
	msgpack_pack_float(&pk, a.acceleration().x_);
	msgpack_pack_float(&pk, a.acceleration().y_);

	if(network->sendRPC(Procedure::VOTE_MOVE_ROBOT, sbuf.data, sbuf.size) < 0) {
		return -1;
	}

	return 0;
}


