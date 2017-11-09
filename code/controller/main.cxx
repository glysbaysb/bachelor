#include <iostream>
#include <math.h>
#include <memory>
#include <experimental/optional>

#include <libnetwork/network.h>

struct Position {
	float x_,
		  y_;

	Position(float x, float y) :
		x_(x), y_(y)
	{
	}

	friend std::ostream& operator<<(std::ostream& os, const Position& p) {
		return os << '(' << p.x_ << ';' << p.y_ << ')';
	}
};

struct Object {
	int id_;
	Position pos_;

	// fixme: rest
	Object(int id, const Position& pos) :
		id_(id), pos_(pos)
	{
	}

	friend std::ostream& operator<<(std::ostream& os, const Object& o) {
		return os << '#' << o.id_ << '\n' <<
			'\t' << o.pos_ << '\n';
	}
};

struct WorldStatus
{
	float xTilt_,
		  yTilt_;
	std::vector<Object> objects;

	friend std::ostream& operator<<(std::ostream& os, const WorldStatus& ws) {
		os << ws.xTilt_ << ';' << ws.yTilt_ << '\n';
		for(auto&& i : ws.objects) {
			os << i;
		}

		return os;
	}

};

static WorldStatus unpackWorldStatus(msgpack_object_array* params);

void worldStatusCallback(void* optional, msgpack_object_array* params)
{
	Network* network = (Network*)optional;

	auto ws = unpackWorldStatus(params);
	std::cout << ws << '\n';

	/* algo */

	/* weg senden */
}

int main(int argc, char** argv)
{
	if(argc < 2) {
		printf("%s interface\n", argv[0]);
		return 0;
	}

	auto network = Network(argv[1]);
	network.addRPCHandler(WORLD_STATUS, &worldStatusCallback, &network);

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
		auto m = object->ptr[4].via.f64;

		assert(object->ptr[5].type == MSGPACK_OBJECT_FLOAT || object->ptr[5].type == MSGPACK_OBJECT_FLOAT32);
		auto fuel = object->ptr[5].via.f64;

		ws.objects.emplace_back(Object(ID, {x, y}));
	}

	return ws;
}
