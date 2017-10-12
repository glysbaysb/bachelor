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
			std::cout << "skip: " << i->ifa_name << '\n';
			continue;
		}

		int sock = -1;
		if((sock = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			s = -1;
		}

		int broadcastEnable=1;
		if(::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
			s = -2;
		}

		sockaddr_in* sin = (sockaddr_in*)i->ifa_broadaddr;
		sin->sin_port = (in_port_t)htons(port);

		char buf[100];
		if(inet_ntop(AF_INET, (const void*)&sin->sin_addr, buf, sizeof(buf)) != nullptr) {
			printf("broadcast: %s\n", buf);
		}

		if(::connect(sock, (const sockaddr*)sin, sizeof(sockaddr_in)) < 0) {
			s = -3;
		}

#ifdef DEBUG
		std::cout << i->ifa_name << ':' << sock << '\n';
#endif
		_broadcastSockets.push_back(sock);
	}

	::freeifaddrs(ifs);

	return s;
}

int ECCUDP::bind(const int16_t port, const char* interface)
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
			std::cout << "skip: " << i->ifa_name << '\n';
			continue;
		}

		int sock = -1;
		if((sock = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			s = -1;
			continue;
		}

		ifreq tmp = {0};
		strncpy(tmp.ifr_name, interface, strlen(interface)+1);
		if(setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char*)&tmp, sizeof(tmp)) < 0) {
			perror("can't setsockop(): ");
			close(sock);
			return -3;
		}

		assert(i->ifa_addr); 
		sockaddr_in* sin = (sockaddr_in*)i->ifa_addr;
		sin->sin_port = (in_port_t)htons(port);

		char buf[100];
		if(inet_ntop(AF_INET, (const void*)&sin->sin_addr, buf, sizeof(buf)) != nullptr) {
			printf("bind: %s\n", buf);
		}

		s = ::bind(sock, (const sockaddr*)sin, sizeof(sockaddr_in));
        if (s == 0) {
			_bindSockets.push_back(sock);
            continue;
        }

        ::close(sock);
    }

	::freeifaddrs(ifs);
	return s < 0 ? 
		_bindSockets.size() ? _bindSockets.size() : // maybe one bind() was successful
			s
		: s;
}
