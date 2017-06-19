#ifndef WORLD_H
#define WORLD_H


/**
 * @brief connects this voter to the simulated world
 *
 * @return: null on error
 */
void* connectToWorld();

// todo
typedef struct WorldStatus {
	int dummy;
} WorldStatus;

/**
 * @brief this function is implemented by the students and gets called periodically
 * 	  with updates about the status of the world.
 *
 * This function gets called peridocally from another thread, so make sure that your
 * functio is reentrant;
 *
 * @param ws: the new WorldStatus
 * @param optional: ca be used to pass along additional user-specific information
 */
typedef void (*TypeGetWorldStatusCallback)(WorldStatus ws, void* optional);

/**
 * @brief start processing events from the world
 *
 * @param ctx: The result rom connectToWorld()
 * @param cb: your function
 * @param optional: data that shall be passed to your function
 */
int startProcessingWorldEvents(void* ctx, TypeGetWorldStatusCallback cb, void* optional);

/**
 * @brief moves that robot in the world
 *
 * @param ctx: the world
 * @param id: the robot to be moved
 * @param diffX: speedup in X difection
 * @param diffY: speedup in Y difrection
 */
void MoveRobot(void* ctx, int id, int diffX, int diffY);

/**
 * @brief detach from the world and stop calling the callback
 *
 * @param ctx: the world context
 */
void detachFromWorld(void* ctx_);

#endif
