#include <iostream>
#include <iterator>
#include <cstdio>
#include <unistd.h>
#include <libworld/world.h>
#include <libnetwork/network.h>

typedef struct {
	int robot;
	void* worldCtx;
	Network* network;
} Info;

void worldStatusCallback(const WorldStatus* ws, void* additional);
void worldStatusCallback(const WorldStatus* ws, void* additional)
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
			if((r = MoveRobot(info->worldCtx, ws->objects[i].id, 1, 1)) < 0) {
				fprintf(stderr, "can't move robot: %d", r);
			}
		}
	}
}

void voteCallback(void* optional, msgpack_object_array* params)
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

int main(int argc, char** argv)
{
	Info callbackInfo = {0};

	if(argc < 3) {
		printf("%s host interface\n", argv[0]);
		return 0;
	}

	callbackInfo.network = new Network(argv[2]);
	callbackInfo.network->addRPCHandler(Procedure::VOTE_MOVE_ROBOT, voteCallback, (void*)&callbackInfo);

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
