#ifndef WORLD_H
#define WORLD_H


/**
 * @brief connects this voter to the simulated world
 *
 * @return: null on error
 */
void* initWorld();

typedef struct WorldStatus {
	int dummy;
	// todo
} WorldStatus;
typedef void (*TypeGetWorldStatusCallback)(WorldStatus ws);

/**
 * @brief start processing events from the world
 */
int startProcessingWorldEvents(void* ctx, TypeGetWorldStatusCallback cb);

#endif
