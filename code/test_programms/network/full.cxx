/**
 * @file a simple application that reads data from stdin, sends them as RPC,
 * receives the reply (exactly what has been sent) and then writes that to
 * stdout.
 *
 * I've used it to test whether the RPC works, with something like:
 * Machine A: `./full`
 * Machine B: `./full < somefile > somefile2`
 * Then `diff somefile somefile2`
 */
#include <iostream>
#include <iterator>
#include <cstdint>
#include <unistd.h>
#include <chrono>
#include <future>

#include <msgpack.h>
#include <libnetwork/network.h>
#include <librpc/rpc.h>

static void pack_msgpack(std::string str, msgpack_sbuffer& sbuf);

void echo_callback(void* optional, msgpack_object_array* params)
{
    if(!params || !params->size) {
        return;
    }

    for(size_t i = 0; i < params->size; i++) {
        switch(params->ptr[i].type) {
        case MSGPACK_OBJECT_ARRAY:
            echo_callback(optional, (msgpack_object_array*) &params->ptr[i].via);
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
	if(argc < 2) {
		std::cout << argv[0] << " interface" << std::endl;
		return 0;
	}

	Network network(argv[1]);

	network.addRPCHandler(Procedure::ECHO, echo_callback, nullptr);

	auto read_line =[](){
		std::string line;
		std::getline(std::cin, line);
		return line;
	};
	auto future = std::async(std::launch::async, read_line);
	while(1) {
		/* stdin data? */
		if(future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			auto line = future.get();
			future = std::async(std::launch::async, read_line);
			
			msgpack_sbuffer sbuf;
			msgpack_sbuffer_init(&sbuf);

			pack_msgpack(line, sbuf);
			if(network.sendRPC(Procedure::ECHO, sbuf.data, sbuf.size) < 0) {
				std::cerr << "can't send rpc\n";
			}
			msgpack_sbuffer_destroy(&sbuf);
		}

		/* network data? */
		network.step();

		sleep(1);
	}

	return 0;
}

static void pack_msgpack(std::string str, msgpack_sbuffer& sbuf)
{
    msgpack_packer pk;

    /* serialize values into the buffer using msgpack_sbuffer_write callback function. */
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_pack_array(&pk, 1);
    msgpack_pack_str(&pk, str.size());
    msgpack_pack_str_body(&pk, str.c_str(), str.size());
}
