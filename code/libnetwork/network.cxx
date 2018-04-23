#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <ifaddrs.h>
#include <string>

#include <libecc/ecc.h>

#include "network.h"


ECCUDP::ECCUDP(int16_t bindPort, int16_t broadcastPort, const char* interface)
{
	ECC::initialize();

	if(bind(bindPort, interface) < 0) {
		throw std::runtime_error("can't bind");
	}

	if(createBroadcastSockets(broadcastPort, interface) < 0) {
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

int ECCUDP::poll(int timeout, std::vector<RecvPacket>& packets)
{
	auto pollfds = std::vector<pollfd>();

	for(auto&& port : _bindSockets) {
		pollfds.push_back(pollfd{port, POLLIN, 0});
	}

	/* as I always go through the whole array of pollfds, there's
	   no point in saving it.
	   todo: don't do that then: abort for loop, if all events have been consume */
	int ret = 0;
	if((ret = ::poll(pollfds.data(), pollfds.size(), timeout)) < 0) {
		perror("poll");
		return -1;
	}

	for(auto&& pollfd : pollfds) {
		if((pollfd.revents & POLLIN) == POLLIN) {
			auto packet = RecvPacket(512);// todo: better size

			auto count = ::recvfrom(pollfd.fd, packet.p.data(), packet.p.capacity(), 0, (sockaddr*)(&packet.addr), &packet.addrLen);
			if (count == 0 || count == -1) {
				if (count == -1 && errno != EAGAIN) {
					perror("recvfrom");
				}
				continue;
			}

			packet.p.resize(count);
			packet.p = ECC::decode(packet.p);
			packets.push_back(packet);
		} else if((pollfd.revents & POLLIN) == POLLERR) {
			ret = -1;
		}
	}

	return ret;
}

int ECCUDP::send(const Packet& data)
{
	const auto encoded = ECC::encode(data);

	int success = 0;
	for(auto&& socket: _broadcastSockets) {
		auto r = ::send(socket, encoded.data(), encoded.size(), 0);
		if(r != encoded.size()) {
			success = -1;
		}
	}

	return success;
}

int ECCUDP::createBroadcastSockets(std::int16_t port, const char* interface)
{
	int s = -1;

	ifaddrs* ifs = nullptr;
	if((s = ::getifaddrs(&ifs)) < 0) {
		return s;
	}

	for(auto i = ifs; i; i = i->ifa_next) {
		/* IPv4 only */
		if(i->ifa_addr->sa_family != AF_INET) {
			continue;
		}

		if(std::string(i->ifa_name) != interface) {
			continue;
		}

		int sock = -1;
		if((sock = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			s = -1;
			goto RET;
		}

		int broadcastEnable=1;
		if(::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
			s = -2;
			goto RET;
		}

		sockaddr_in* sin = (sockaddr_in*)i->ifa_broadaddr;
		sin->sin_port = (in_port_t)htons(port);

		char buf[100];
		if(inet_ntop(AF_INET, (const void*)&sin->sin_addr, buf, sizeof(buf)) == nullptr) {
			s = -3;
			goto RET;
		}

		if(::connect(sock, (const sockaddr*)sin, sizeof(sockaddr_in)) < 0) {
			s = -4;
			goto RET;
		}

#ifdef DEBUG
		std::cout << i->ifa_name << ':' << sock << '\n';
#endif
		_broadcastSockets.push_back(sock);
	}

RET:
	::freeifaddrs(ifs);

	return s;
}

int ECCUDP::bind(const int16_t port, const char* interface)
{
    addrinfo hints;
    addrinfo *result, *rp;
    int s, sfd = -1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_DGRAM; /* We want a UDP socket */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;     /* All interfaces */

    s = ::getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
    if (s != 0) {
        perror("getaddrinfo: ");
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        s = ::bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
			_bindSockets.push_back(sfd);
            /* We managed to bind successfully! */
            /* fixme: find some way to bind on ip4 and 6 - continue and pass array?*/
            break;
        }

        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    ::freeaddrinfo(result);

    return sfd;
}
