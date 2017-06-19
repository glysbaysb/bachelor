#ifndef WORLD_H
#define WORLD_H


/**
 * @brief connects this voter to the simulated world
 *
 * @return: null on error
 */
void* connectToWorld();

typedef struct WorldStatus {
	int dummy;
	// todo
} WorldStatus;

/**
 * @brief this function is implemented by the students and gets called periodically
 * 	  with updates about the status of the world
 *
 * @param ws: the new WorldStatus
 * @param optional: ca be used to pass along additional user-specific information
 */
typedef void (*TypeGetWorldStatusCallback)(WorldStatus ws, void* optional);

/**
 * @brief start processing events from the world
 *
 * @param ctx: The result rom connectToWorld()
 */
int startProcessingWorldEvents(void* ctx, TypeGetWorldStatusCallback cb);

/**
 * @brief moves that robot in the world
 *
 * @param ctx: the world
 * @param id: the robot to be moved
 * @param diffX: speedup in X difection
 * @param diffY: speedup in Y difrection
 */
void MoveRobot(void* ctx, int id, int diffX, int diffY);


#endif
