#include <libnetwork/network.h>
#include <simulation/world.h>

struct WorldStatus
{
	float xTilt_,
		  yTilt_;
	std::vector<Object> objects;
};

static WorldStatus unpackWorldStatus(msgpack_object_array* params);

void worldStatusCallback(void* optional, msgpack_object_array* params)
{
	Network* network = (Network*)optional;

	unpackWorldStatus();

	/* algo */

	/* weg senden */
}

int main(int argc, char** argv)
{
	if(argc < 2) {
		printf("%s interface\n", argv[0]);
		return 0;
	}

	auto network = Network(argv[1]);
	network.addRPCHandler(WORLD_STATUS, &worldStatus, &network);

	while(1)
	{
	}

	return 0;
}

static WorldStatus unpackWorldStatus(msgpack_object_array* params)
{
/**
 * WorldStatus is
 * - Angle (float, float)
 * - Array of Object
 *
 * Object is:
 * - ID (int)
 * - type (int)
 * - x, y (float)
 * - m (float)
 * - fuel (int)
 */
}
