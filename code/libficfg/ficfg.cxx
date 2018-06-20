#include <iostream>
#include <fstream>
#include <msgpack.h>

#include <libworld/world.h>

#include "ficfg.h"

static void fakeObject(SimulationObject* o) {
	const int sign = rand() % 2 == 0 ? 1 : -1;

	switch(rand() % 5) {
	case 0:
		o->x += sign * (rand() % 50000) / 10000.;
		break;
	case 1:
		o->y += sign * (rand() % 50000) / 10000.;
		break;
	case 2:
		o->rotation += sign * (rand() % 70000) / 10000.;
		break;
	case 3:
		o->m += sign * (rand() % 50000) / 10000.;
		break;
	case 4:
		o->fuel += sign * (rand() % 50);
		break;
	}
}

void fakeWorldStatus(WorldStatus* ws) {
	const int sign = rand() % 2 == 0 ? 1 : -1;

	switch(rand() % 5) {
	case 0:
	case 1:
		ws->xTilt += sign * (rand() % 20000) / 10000.;
		break;
	case 2:
	case 3:
		ws->yTilt += sign * (rand() % 20000) / 10000.;
		break;
	case 4: {
		size_t idx = rand() % ws->numOfObjects;
		SimulationObject * o = &ws->objects[idx];
		fakeObject(o);

		break;
	}
	}

}

void getFiCfgCallback(void* optional, msgpack_object_array* params) {
	FiCfg* cfg = (FiCfg*)optional;

	assert(params->size == 3);
	assert(params->ptr[2].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[2].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	assert(params->ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[1].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	assert(params->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[0].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);

	cfg->dropWorldStatus = params->ptr[2].via.i64;
	cfg->dupWorldStatus = params->ptr[0].via.i64;
	cfg->fakeWorldStatus = params->ptr[1].via.i64;
}

