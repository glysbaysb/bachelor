#ifndef WORLD_H
#define WORLD_H

class Position {
private:
	int32_t _x,
		_y;
public:
	Position(int32_t x, int32_t y) : _x(x), _y(y) {
	}
	
	std::pair<int32_t, int32_t> get() {
		return std::pair<int32_t, int32_t>(_x, _y);
	}
	
	// todo: operator overload or whatever and then replace Object:getPosition()?
};

/**
 * @brief base class for all objects
 */
class Object {
private:
	Position p_;
	int32_t dimension_;

public:
	// todo: virtual deconstructor
	
	Object(Position p, int32_t dimension) : p_(p), dimension_(dimension) {
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
	static const int DIMENSION = 2;
	
	/**
	 * @brief a fuel source cannot move, so calling this function does nothing
	 */
	void move() {
		;
	}
	
	FuelSource(Position p) : Object(p, DIMENSION) {
	}
};

class Robot : public Object {
private:
	int8_t fuelStatus_; // 0 <= fuelStatus_ <= 100
	
	int32_t id_;
	static int32_t GLOBAL_ID; //! 0 at programm startup, incremented for each invocation of constructor
public:
	static const int DIMENSION = 3;

	Robot(Position p) : Object(p, DIMENSION), id_(GLOBAL_ID++) {
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
	
	int32_t getID() {
		return id_;
	}
};


class World {
private:
	std::vector<Robot> robots_;
	std::unique_ptr<FuelSource> fuelSource_;
	
	int32_t dimension_;
	
public:
	// getRobot() / getRobots()
	// getFuelSource()
	
	/**
	 * @brief creates an empty world
	 *
	 */
	World(int32_t dimension);
	
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
	
	std::pair<int32_t, int32_t> getDimensions() {
		return std::pair<int32_t, int32_t>(dimension_, dimension_);
	}
};

#endif // WORLD_H
