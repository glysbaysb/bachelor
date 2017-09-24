#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <cstdint>

/**
 * @brief A helper class to abstract away implementation details
 */
typedef std::vector<std::uint8_t> Packet;

class ECCUDP
{
private:
	std::vector<int> _bindSockets; //! all sockets this class listens on. Maybe it's one for IPv4, another for IPv6
	int _broadcastSocket; //! the socket this class uses to send broadcasts. It is connected(), because the destinaon port is always the same

	/**
	 * @brief binds on all possible network interfaces, for all possible families.
	 *
	 * the results are stored in @_bindSockets.
	 *
	 * @todo: bind only for some network interfaces?
	 *
	 * @return 0 on success, < 0 in case an error happend
	 */
	int bind(const char *port);

	/**
	 * @brief creates a socket, give it broadcast permissions and connect() it to that port
	 *        so there's no need to set it before every send()
	 *
	 * @return 0 on success, < 0 in case an error happend
	 */
	int createBroadcastSocket(std::int16_t port);
public:
	/**
	 * @brief bind to that port & create the broadcast socket
	 *
	 * @throws May throw an exception, in case an error happend
	 */
	ECCUDP(const char* bindPort, const char* broadcastPort);

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

	int poll(int timeout, std::vector<Packet>& packets);
};

#endif // NETWORK_H
