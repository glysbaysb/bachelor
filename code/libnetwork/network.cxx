#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <cassert>

#include "network.h"


ECCUDP::ECCUDP(const char* bindPort, const char* broadcastPort)
{
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

int ECCUDP::send(const Packet& data)
{
	auto r = ::send(_broadcastSocket, data.data(), data.size(), 0);
	return r == data.size() ? 0 : (
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
			_bindSockets.push_back(s);
            break;
        }

        ::close(sfd);
    }

    if (rp == NULL) {
        return -2;
    }

    ::freeaddrinfo(result);

    return sfd;
}
