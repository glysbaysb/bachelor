#ifndef WORLD_H
#define WORLD_H


/**
 * @brief connects this voter to the simulated world
 *
 * @return: null on error
 */
void* connectToWorld(void);

/**
 * All the information needed to describe a object in the simulation
 */
typedef struct SimulationObject {
	char* type; //! Either "ROBOT" or "FUELSTATION"

	float x; //! x position in the world
	float y; //! y position
	float m; //! mass of object

	int id; //! id of this object. Use that for MoveRobot() calls

	int fuel; //! how much fuel that robot has left. invalid for a
		  //! fuel station
} SimulationObject;

/**
 * The state of the whole simulation. E.g. all objects
 */
typedef struct WorldStatus {
	size_t numOfObjects;
	SimulationObject* objects;
} WorldStatus;

/**
 * @brief this function is implemented by the students and gets called periodically
 * 	  with updates about the status of the world.
 *
 * As soon as your function returns ws gets freed, so if you need the information
 * provided there you should save it somewhere else. Using ws afterwards *will*
 * fail!
 *
 * @param ws: the new WorldStatus
 * @param optional: ca be used to pass along additional user-specific information
 */
typedef void (*TypeGetWorldStatusCallback)(const WorldStatus* ws, void* optional);

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
