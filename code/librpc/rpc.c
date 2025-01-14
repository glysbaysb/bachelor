#include <stdio.h>
#include <msgpack.h>
#include <msgpack/object.h>

#include "rpc.h"

enum Operation {
	REQUEST = 0,
	RESPONSE = 1,
};

typedef struct RPC {
	enum Operation op;
	int id;
	union {
		int error;
		enum Procedure procedure;
	};
	msgpack_object_array* params;
} RPC;

typedef struct RPCContext {
	int id;

	size_t numOfProcedures;
	RPCProcedure* procedures;

	size_t numRPCsInFlight;
	RPCInFlight* rpcsInFlight;
} RPCContext;

static int parseRPC(struct RPC* rpc, struct msgpack_unpacker* unpacker, msgpack_unpacked* result) {
	/*const char* typeToStr[] = {"nil", "boolean", "pos int", "neg int",
					"float", "str", "array", "map", "bin", "ext"};*/
	int ret = msgpack_unpacker_next(unpacker, result);
	if (ret != MSGPACK_UNPACK_SUCCESS) {
		return -1;
	}
	msgpack_object obj = result->data;

	switch(obj.type) {
	case MSGPACK_OBJECT_ARRAY:{
		msgpack_object_array* arr = (msgpack_object_array*)&obj.via;
		if(arr->size < 4) {
			fprintf(stderr, "array too small");
			break;
		}

		assert(arr->ptr[0].type == MSGPACK_OBJECT_POSITIVE_INTEGER);
		rpc->op = (int)(arr->ptr[0].via.i64 & INT32_MAX);
		assert(arr->ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER);
		rpc->id = (int)(arr->ptr[1].via.i64 & INT32_MAX);
		assert(arr->ptr[2].type == MSGPACK_OBJECT_POSITIVE_INTEGER);
		rpc->error = (int)(arr->ptr[2].via.i64 & INT32_MAX);

		assert(arr->ptr[3].type == MSGPACK_OBJECT_ARRAY);
		rpc->params = ((msgpack_object_array*)&arr->ptr[3].via);

		ret = 0;
		break;
	}
	default:
		fprintf(stderr, "unhandled msgpack type:");
		msgpack_object_print(stderr, obj);
		putchar('\n');
		ret = -1;
		break;
	}


	return ret;
}

int handleRPC(void* rpc_, const unsigned char* buf, const size_t len) {
	int r = -1;
	RPCContext* rpc = (RPCContext*)rpc_;
	struct RPC parsed;

	/* initalize a large enough buffer to unpack into */
	msgpack_unpacker* unpacker = msgpack_unpacker_new(len);
	if(unpacker == NULL)
		return r;
	msgpack_unpacker_reserve_buffer(unpacker, len); // really, really reserve that much.
	assert(msgpack_unpacker_buffer_capacity(unpacker) >= len);

	/* prepare the buffer */
	memcpy(msgpack_unpacker_buffer(unpacker), buf, len);
	msgpack_unpacker_buffer_consumed(unpacker, len);

	/* and finally unpack */
	msgpack_unpacked result;
	msgpack_unpacked_init(&result);

	if((r = parseRPC(&parsed, unpacker, &result)) < 0) {
		fprintf(stderr, "can't parse rpc parsed: %d\n", r);

		msgpack_unpacked_destroy(&result);
		msgpack_unpacker_free(unpacker);

		return -1;
	}

#ifdef DEBUG
	printf("rpc: %s id: %d\n", parsed.op == RESPONSE ? "RESPONSE" : "REQUEST", parsed.id);
#endif
	if(parsed.op == RESPONSE) {
		for(size_t i = 0; i < rpc->numRPCsInFlight; i++) {
			if(rpc->rpcsInFlight[i].id != parsed.id)
				continue;

			rpc->rpcsInFlight[i].proc.proc(rpc->rpcsInFlight[i].proc.optional, parsed.params);
			r = 0;
		}
	} else if(parsed.op == REQUEST) {
		for(size_t i = 0; i < rpc->numOfProcedures; i++) {
			if(rpc->procedures[i].num != parsed.procedure)
				continue;

			rpc->procedures[i].proc(rpc->procedures[i].optional, parsed.params);
			r = 0;
		}
	}

	msgpack_unpacked_destroy(&result);
	msgpack_unpacker_free(unpacker);
	return r;
}

void* createRPCContext(void) {
	RPCContext* rpc = (RPCContext*)calloc(1, sizeof(RPCContext));
	return rpc;
}

void destroyRPCContext(void* rpc_) {
	RPCContext* rpc = (RPCContext*)rpc_;

	free(rpc->procedures);
	free(rpc->rpcsInFlight);
	free(rpc);
}

int addProcedure(void* rpc_, const enum Procedure num, const TypeRPCProcedure proc, void* optional) {
	RPCContext* rpc = (RPCContext*)rpc_;

	void* new = realloc(rpc->procedures, (rpc->numOfProcedures + 1) * sizeof(RPCProcedure));
	if(!new)
		return -1;
  
	rpc->procedures = new; 
	RPCProcedure tmp = {.num = num,
		.proc = proc,
		.optional = optional
	};
	memcpy(&rpc->procedures[rpc->numOfProcedures], &tmp, sizeof(tmp));
	rpc->numOfProcedures++;

	return 0;
}

int getRegisteredProcedures(const void* rpc_, RPCProcedure* arr, const size_t sizeOfArr) {
	const RPCContext* rpc = (const RPCContext*)rpc_;

	if(sizeOfArr < rpc->numOfProcedures)
		return -1;

	memcpy(arr, rpc->procedures, sizeof(RPCProcedure) * rpc->numOfProcedures);
	return 0;
}

static int addRequestToInFlightList(RPCContext* rpc, const enum Procedure num, const int id) {
	bool found = false;
	for(size_t i = 0; i < rpc->numOfProcedures; i++) {
		if(rpc->procedures[i].num != num)
			continue;

		void* new = realloc(rpc->rpcsInFlight, (rpc->numRPCsInFlight + 1) * sizeof(RPCInFlight));
		if(!new)
			return -1;

		rpc->rpcsInFlight = new;
		RPCInFlight tmp = {.id = id,
			.proc = rpc->procedures[i]
		};
		memcpy(&rpc->rpcsInFlight[rpc->numRPCsInFlight], &tmp, sizeof(tmp));
		rpc->numRPCsInFlight++;

		found = true;
		break;
	}

	if(!found) {
		fprintf(stderr, "there was no procedure registered for %d\n", num);
	}

	return found ? 0 : -2;
}

int getRPCsInFlight(const void* rpc_, RPCInFlight* arr, const size_t sizeOfArr) {
	const RPCContext* rpc = (const RPCContext*)rpc_;

	if(sizeOfArr < rpc->numRPCsInFlight) {
		return -1;
	}

	memcpy(arr, rpc->rpcsInFlight, sizeof(RPCInFlight) * rpc->numRPCsInFlight);
	return 0;
}

int createRPCRequest(void* rpc_, const enum Procedure num, const void* paramsBuffer, size_t paramsLen, void** outBuffer, size_t* outBufferLen)
{
	static const uint8_t EMPTY_MSGPACK_ARRAY[] = {0x90};
	RPCContext* rpc = (RPCContext*)rpc_;

	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 4);
	msgpack_pack_int32(&pk, REQUEST); // operation
	msgpack_pack_int32(&pk, ++rpc->id); // id
	msgpack_pack_int32(&pk, num); // procedure

	/* while the caller can signify that this request has no parameters by passing a nullptr,
	   the reciever always expects an parameter array.
	   So, if neccassary, create that */
	if(!paramsLen) {
		paramsBuffer = EMPTY_MSGPACK_ARRAY;
		paramsLen = sizeof(EMPTY_MSGPACK_ARRAY);
	}

#if 0
	for(size_t i = 0; i < sbuf.size; i++) {
		printf("%02X ", (sbuf.data[i] & 0xFF));
	}
	putchar('\n');
#endif

	if(!(*outBuffer = calloc(1, sbuf.size + paramsLen))) {
		msgpack_sbuffer_destroy(&sbuf);
		return -1;
	}
	memcpy(*outBuffer, sbuf.data, sbuf.size);
	memcpy((void*)((uintptr_t)*outBuffer + sbuf.size), paramsBuffer, paramsLen);
	*outBufferLen = sbuf.size + paramsLen;

	if(addRequestToInFlightList(rpc, num, rpc->id) < 0) {
		msgpack_sbuffer_destroy(&sbuf);
		return -2;
	}

	msgpack_sbuffer_destroy(&sbuf);
	return 0;
}
