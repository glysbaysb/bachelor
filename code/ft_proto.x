program FT_PROTO {
	version FT_PROTO_VERSION {
		void MoveRobot(MoveRequest) = 1;
		WorldStatus GetStatus(void) = 2;
	} = 1;
} = 0x20000001;
	
const N = 10; /* number of all controllers */
const T =  3; /* max traitors */

enum ObjectType {
  ROBOT = 0,
  FUEL_SOURCE = 1
};

struct Object {
  enum ObjectType type;
  int x;
  int y;
  int m;
  
  /* velocity? */
  
  int id;
  int fuel; /* only valid for a robot, not for fuel sources*/
};

/** Controler -> controller
 * used to generate consens about the state of the world
 */
const MAXOBJECTS = 100;
struct ObjectMessageTree {
  Object objects;
  int sender<N>;
};

/* Controller -> Voter */
struct MoveRequest {
  int id;
  int diffX;
  int diffY;
};
