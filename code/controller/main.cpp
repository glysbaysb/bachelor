#include <libnetwork/network.h>

int main(int argc, char** argv)
{
	if(argc < 2) {
		printf("%s interface\n", argv[0]);
		return 0;
	}

	auto network = Network(argv[1]);
	network.addRPCHandler(WORLD_STATUS, &worldStatus);

	while(1)
	{
	}

	return 0;
}

void worldStatus()
{
	/* */

	/* run algo */

	/* send out actions */
	/* or: send to main? Then from there do consensus */
}

/**
  * ist am einfachsten.
  * aber auch langweilig
  */
void method1()
{
	while(1) {
		/* get world status */

		/* does that match with expection?*/
		/* else: restart */

		/* determine next step for robot X */

		/* does that seem like a valid result? */
		/* send to voter */
		/* else: restart */
	}
}

/**
  * eigentlich auch einfach.
  * bei "gather results" muesste man in einer schleife poll aufrufen (mit immer kleiner werdenen
  * intervallen) um dort nicht zu viel Zeit zu verlieren
  *
  * aber hier wird main() vlt extrem lang und unstrukturiert
  */
void method2()
{
	while(1) {
		/* get world status */

		/* does that match with expection?*/
		/* else: restart */

		/* determine next step for robot X */

		/* start consensus */

		/* gather results */

		/* decide */

		/* send to voter */
	}
}

/**
  * so ne statemachine ist zwar cool, aber die uebergaenge sind schwer.
  * Ein uebergang ist ja entweder nach einem ereigniss ODER nach einer bestimmten
  * zeit und wahrscheinlich macht es auch sinn immer utnerschiedliche zeiten fuer
  * die utnerschiedlichen moeglichkeiten zu haben
  */
enum STATES = {
	WAITING_FOR_WORLD_STATUS = 0,
	CALCULATE_GOOD_MOVMENTS,
	START_CONSENSUS,
	GET_CONSENSUS,
	SEND_RESULT
};

void method3()
{
	int state = WAITING_FOR_WORLD_STATUS;
	while(1) {
		switch(state) {
		case WAITING_FOR_WORLD_STATUS:
			/* get world status */
			break;
		case CALCULATE_GOOD_MOVEMENTS:
			/* determine next step for robot X */
			break;
		case START_CONSENSUS:
			/* start consensus */
			break;
		case GET_CONSENSUS:
			/* gather results */
			break;
		case SEND_RESULT:
			/* decide */
			/* send to voter */
			break;
		}
	}
}
