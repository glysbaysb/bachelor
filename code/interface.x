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

/* World -> Voter */
const MAXOBJECTS = 100;
struct WorldStatus {
  Object objects<MAXOBJECTS>;
};

/* Voter -> World */
struct MoveRequest {
  int id;
  int diffX;
  int diffY;
};

program SIMULATION {
	version SIMULATION_VERSION {
		void MoveRobot(MoveRequest) = 1;
		WorldStatus GetStatus(void) = 2;
	} = 1;
} = 0x20000001;
