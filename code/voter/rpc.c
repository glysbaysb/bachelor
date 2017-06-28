#include <stdio.h>
#include <msgpack.h>

#include "rpc.h"

void parseRPCReply(char* buf, size_t len, struct RPCReply* reply) {
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

			msgpack_object_array* paramArr = ((msgpack_object_array*)&arr->ptr[3].via);
			size_t numOfParams = paramArr->size;
			if(!(reply->params = calloc(numOfParams, sizeof(int)))) {
				fprintf(stdout, "can't alloc for params");
				break;
			}
			for(size_t i = 0; i < numOfParams; i++) {
				reply->params[i] = paramArr->ptr[i].via.i64;
			}

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
