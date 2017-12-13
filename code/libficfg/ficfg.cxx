#include <iostream>
#include <fstream>
#include <msgpack.h>

#include <libworld/world.h>

#include "ficfg.h"

void fakeWorldStatus(WorldStatus* ws) {
	switch(rand() % 5) {
	case 0:
	case 1:
		ws->xTilt += (rand() % 20000) / 10000.;
		break;
	case 2:
	case 3:
		ws->yTilt += (rand() % 20000) / 10000.;
		break;
	case 4:
		/*				ws->numOfObjects;
			ws->objects;*/
		break;
	}

}

void getFiCfgCallback(void* optional, msgpack_object_array* params) {
	FiCfg* cfg = (FiCfg*)optional;

	assert(params->size == 3);
	assert(params->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[0].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	cfg->dropWorldStatus = params->ptr[0].via.i64;
	assert(params->ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[1].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	cfg->dupWorldStatus = params->ptr[1].via.i64;
	assert(params->ptr[2].type == MSGPACK_OBJECT_POSITIVE_INTEGER || params->ptr[2].type == MSGPACK_OBJECT_NEGATIVE_INTEGER);
	cfg->fakeWorldStatus = params->ptr[2].via.i64;
}

