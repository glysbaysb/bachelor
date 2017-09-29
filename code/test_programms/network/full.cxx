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

class Network {
protected:
	void* rpc;
	ECCUDP udp;
public:
	Network() : udp(7777, 7777, "eth1")
	{
		if((rpc = createRPCContext()) == nullptr) {
			throw "can't create RPC context";
		}
	}

	int addRPCHandler(Procedure num, TypeRPCProcedure handler, void* optional)
	{
		return addProcedure(rpc, num, handler, optional);
	}

	/* todo: find a better name */
	void step()
	{
		auto packets = std::vector<Packet>();
		if(udp.poll(0, packets) < 0) {
			return;
		}
		
		for(auto&& i : packets) {
			for(auto j = 0; j < i.size(); j++) {
				printf("%d ", i.at(j));
			}
			putchar('\n');

			handleRPC(rpc, i.data(), i.size());
		}
	}

	int sendRPC(Procedure num, const void* paramsBuffer, size_t paramsLen)
	{
		uint8_t* buffer;
		size_t bufferLen;
		int r;
		if((r = createRPCRequest(rpc, num, paramsBuffer, paramsLen, (void**)&buffer, &bufferLen)) < 0) {
			return r;
		}

		auto p = Packet{buffer, buffer + bufferLen};
		free(buffer);
		return udp.send(p);
	}

	~Network()
	{
		destroyRPCContext(rpc);
	}
};

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
	Network network;

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
