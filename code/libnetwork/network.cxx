#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <libecc/ecc.h>

#include "network.h"


ECCUDP::ECCUDP(const char* bindPort, const char* broadcastPort)
{
	ECC::initialize();

	if(bind(bindPort) < 0) {
		throw std::runtime_error("can't bind");
	}

	auto iBroadcastPort = std::stoi(broadcastPort);
	assert(iBroadcastPort > 0 && iBroadcastPort < INT16_MAX);
	if((_broadcastSocket = createBroadcastSocket(iBroadcastPort)) < 0) {
		throw std::runtime_error("can't create broadcast socket");
	}
}

ECCUDP::~ECCUDP()
{
	for(auto&& socket: _bindSockets) {
		::close(socket);
	}

	::close(_broadcastSocket);
}

int ECCUDP::poll(int timeout, std::vector<Packet>& packets)
{
	auto pollfds = std::vector<pollfd>();

	for(auto&& port : _bindSockets) {
		pollfds.push_back(pollfd{port, POLLIN, 0});
	}

	/* as I always go through the whole array of pollfds, there's
	   no point in saving it.
	   todo: don't do that then: abort for loop, if all events have been consume */
	if(::poll(pollfds.data(), pollfds.size(), timeout) < 0) {
		return -1;
	}

	int ret = 0;
	for(auto&& pollfd : pollfds) {
		if((pollfd.revents & POLLIN) == POLLIN) {
			auto packet = Packet(512);// todo: better size
			struct sockaddr_storage addr; // should be recvable from both ipv6 and 6
			socklen_t addrLen = sizeof(addr);

			auto count = ::recvfrom(pollfd.fd, packet.data(), packet.capacity(), 0, (sockaddr*)(&addr), &addrLen);
			if (count == 0 || count == -1) {
				if (count == -1 && errno != EAGAIN) {
					perror("recvfrom");
				}
				continue;
			}

			packet.resize(count);
			packets.push_back(ECC::decode(packet));
		} else if((pollfd.revents & POLLIN) == POLLERR) {
			ret = -1;
		}
	}

	return ret;
}

int ECCUDP::send(const Packet& data)
{
	const auto encoded = ECC::encode(data);
	auto r = ::send(_broadcastSocket, encoded.data(), encoded.size(), 0);
	return r == encoded.size() ? 0 : (
				r < 0 ? r : // an error occured
						-r // only parts of the packet were sent
			);
}

int ECCUDP::createBroadcastSocket(std::int16_t port)
{
	int s = -1;
	if((s = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return -1;
	}

	int broadcastEnable=1;
	if(::setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
		return -2;
	}

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = (in_port_t)htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if(::connect(s, (const sockaddr*)&sin, sizeof(sin)) < 0) {
		return -3;
	}

	return s;
}

int ECCUDP::bind(const char *port)
{
    struct addrinfo hints = {0};
    struct addrinfo *result, *rp;
    int s, sfd = -1;

    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_DGRAM; /* We want a UDP socket */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; /* All interfaces -- todo: */

    s = ::getaddrinfo(nullptr, port, &hints, &result);
    if (s != 0) {
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        s = ::bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
			_bindSockets.push_back(sfd);
            continue;
        }

        ::close(sfd);
    }

    ::freeaddrinfo(result);

    return 0;
}
