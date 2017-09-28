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
#include <ifaddrs.h>

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
	if(createBroadcastSockets(iBroadcastPort) < 0) {
		throw std::runtime_error("can't create broadcast socket");
	}
}

ECCUDP::~ECCUDP()
{
	for(auto&& socket: _bindSockets) {
		::close(socket);
	}

	for(auto&& socket: _broadcastSockets) {
		::close(socket);
	}
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

	int success = true;
	for(auto&& socket: _broadcastSockets) {
		auto r = ::send(socket, encoded.data(), encoded.size(), 0);
		if(r != encoded.size()) {
			success = false;
		}
	}

	return success;
}

int ECCUDP::createBroadcastSockets(std::int16_t port)
{
	int s = -1;

	ifaddrs* ifs = nullptr;
	if((s = getifaddrs(&ifs)) < 0) {
		return s;
	}

	for(auto i = ifs; i; i = i->ifa_next) {
		/* IPv4 / v6 only */
		if(i->ifa_addr->sa_family != AF_INET && i->ifa_addr->sa_family != AF_INET6) {
			continue;
		}

		/* todo: fixme: HACK / HACK / HACK */
		if(std::string(i->ifa_name) != "eth1") {
			std::cout << "skip: " << i->ifa_name << '\n';
		}
		/* HACK / HACK / HACK */

		int sock = -1;
		if((sock = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			s = -1;
		}

		int broadcastEnable=1;
		if(::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
			s = -2;
		}

		if(i->ifa_addr->sa_family == AF_INET) {
			sockaddr_in* sin = (sockaddr_in*)i->ifa_broadaddr;
			sin->sin_port = (in_port_t)htons(port);

			if(::connect(sock, (const sockaddr*)sin, sizeof(sockaddr_in)) < 0) {
				s = -3;
			}
		} else {
			sockaddr_in6* sin = (sockaddr_in6*)i->ifa_broadaddr;
			sin->sin6_port = (in_port_t)htons(port);

			if(::connect(sock, (const sockaddr*)sin, sizeof(sockaddr_in6)) < 0) {
				s = -3;
			}
		}

#if DEBUG
		std::cout << i->ifa_name << ':' << sock << '\n';
#endif
		_broadcastSockets.push_back(sock);
	}

	freeifaddrs(ifs);

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
}
