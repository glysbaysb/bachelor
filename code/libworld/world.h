#ifndef WORLD_H
#define WORLD_H
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief connects this voter to the simulated world
 *
 * @return: null on error
 */
void* connectToWorld(const char* host);

/**
* @brief creates a new robot in the world specified by ctx
*
* @param ctx: the world, result of connectToWorld()
* @return: 0 on error
*/
int createRobot(void* ctx);

/**
 * All the information needed to describe a object in the simulation
 */
enum SimulationObjectType {
	ROBOT,
	FUEL_STATION
};
typedef struct SimulationObject {
	enum SimulationObjectType type; //! Either "ROBOT" or "FUELSTATION"

	float x; //! x position in the world
	float y; //! y position
	float rotation; //! rotation. 0 < rotation < 360
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

	float xTilt,
		  yTilt;
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
 * @param speed: 0 <= speed <= 100
 * @param angle: -30 <= angle <= 30
 *
 * @return: 0 on success. -1 for allocation errors, -2 for network errs
 */
int moveRobot(void* ctx, int id, int speed, int angle);

/**
 * @brief detach from the world and stop calling the callback
 *
 * @param ctx: the world context
 */
void detachFromWorld(void* ctx_);

#ifdef __cplusplus
}
#endif
#endif
