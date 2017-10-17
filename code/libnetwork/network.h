#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <cstdint>

#include <librpc/rpc.h>

/**
 * @brief A helper class to abstract away implementation details
 */
typedef std::vector<std::uint8_t> Packet;

class ECCUDP
{
private:
	std::vector<int> _bindSockets; //! all sockets this class listens on. Maybe it's one for IPv4, another for IPv6
	std::vector<int> _broadcastSockets; //! the sockets used to send broadcasts. They're connected(), because the destinaon port is always the same

	/**
	 * @brief binds on that network interface, for all possible families.
	 *
	 * the results are stored in @_bindSockets.
	 *
	 * @return 0 on success, < 0 in case an error happend
	 */
	int bind(const int16_t port, const char* interface);

	/**
	 * @brief creates a socket, give it broadcast permissions and connect() it to that port
	 *        so there's no need to set it before every send()
	 *
	 * @return 0 on success, < 0 in case an error happend
	 */
	int createBroadcastSockets(std::int16_t port, const char* interface);
public:
	/**
	 * @brief bind to that port & create the broadcast socket
	 *
	 * @throws May throw an exception, in case an error happend
	 */
	ECCUDP(int16_t bindPort, int16_t broadcastPort, const char* interface);

	ECCUDP(int16_t bindPort, int16_t broadcastPort) :
		ECCUDP(bindPort, broadcastPort, "eth0") 
	{
	}

	/**
	 * @brief closes all sockets
	 */
	~ECCUDP();

	/**
	 * @brief sends @data over the broadcast socket
	 *
	 * @return returns 0 if the packet was sucessfully sent, something < 0 if not
	 */
	int send(const Packet& data);

	/**
	 * @brief poll() for @timeout and return the results
	 *
	 * @param timeout timeout in milliseconds
	 * @param packets output parameter
	 *
	 * @return negative value if there was an error
	 */
	int poll(int timeout, std::vector<Packet>& packets);
};

class Network {
protected:
	void* rpc;
	ECCUDP udp;

	void _handlePackets(const std::vector<Packet>& packets)
	{
		for(auto&& i : packets) {
			handleRPC(rpc, i.data(), i.size());
		}
	}

public:
	Network(const char* interface) : udp(7777, 7777, interface)
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
			puts("err udp.poll()");
			return;
		}
		
		_handlePackets(packets);
	}

	void poll(int timeout)
	{
		auto packets = std::vector<Packet>();
		if(udp.poll(timeout, packets) < 0) {
			puts("err udp.poll()");
			return;
		}
		
		_handlePackets(packets);
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

#endif // NETWORK_H
