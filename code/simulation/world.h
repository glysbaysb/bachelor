#ifndef WORLD_H
#define WORLD_H

class Position {
private:
	int _x,
		_y;
public:
	Position(int x, int y) : _x(x), _y(y) {
	}
	
	std::pair<int, int> get() {
		return std::pair<int, int>(_x, _y);
	}
	
	// todo: operator overload or whatever and then replace Object:getPosition()?
};

/**
 * @brief base class for all objects
 */
class Object {
private:
	Position p_;
	int dimension_;

public:
	// todo: virtual deconstructor
	
	Object(Position p) : p_(p) {
	}

	std::pair<int, int> getPosition() {
		return p_.get();
	}
	
	int getDimension() {
		return dimension_;
	}
	
	// todo: args?
	virtual void move() = 0;
};

class FuelSource : public Object {
public:
	/**
	 * @brief a fuel source cannot move, so calling this function does nothing
	 */
	void move() {
		;
	}
	
	FuelSource(Position p) : Object(p) {
	}
};

class Robot : public Object {
private:
	int8_t fuelStatus_; // 0 <= fuelStatus_ <= 100
	
	uint32_t id_;
	static uint32_t GLOBAL_ID; //! 0 at programm startup, incremented for each invocation of constructor
	
public:
	Robot(Position p) : Object(p), id_(GLOBAL_ID++) {
	}
	
	int8_t getFuelStatus() {
		return fuelStatus_;
	}
	
	void setFuelStatus(int8_t fuelStatus) {
		assert(fuelStatus >= 0 && fuelStatus <= 100);
		fuelStatus_ = fuelStatus;
	}
	
	void move() {
		assert(false);
	}
	
	uint32_t getID() {
		return id_;
	}
	
	static const int ROBOT_DIMENSION = 1;
};


class World {
private:
	std::vector<Robot> robots_;
	std::unique_ptr<FuelSource> fuelSource_;
	
	int8_t dimension_;
	
public:
	// getRobot() / getRobots()
	// getFuelSource()
	
	/**
	 * @brief creates an empty world
	 *
	 */
	World(int8_t dimension);
	
	~World() {
	}
	
	/**
	 * adds an robot to {world} at position {p}
	 * 
	 * @return either the unique identifier of that robot or a negative value, in 
	 * case of an error
	 */
	int addRobot(Position p);
	
	/**
	 *
	 */
	int addFuelSource(Position p);

	/**
	 *
	 */
	float getWorldDegree();
	
	/**
	 * @brief returns the robots currently in this world
	 */
	std::vector<Robot> getRobots(); 
	
	std::pair<uint32_t, uint32_t> getDimensions() {
		return std::pair<uint32_t, uint32_t>(dimension_, dimension_);
	}
};

#endif // WORLD_H