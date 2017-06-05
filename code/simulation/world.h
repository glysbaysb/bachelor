#ifndef WORLD_H
#define WORLD_H

class Position {
private:
	int32_t _x,
		_y;
public:
	Position(const int32_t x, const int32_t y) : _x(x), _y(y) {
	}
	
	std::pair<int32_t, int32_t> get() const {
		return std::pair<int32_t, int32_t>(_x, _y);
	}
	
	// todo: operator overload or whatever and then replace Object:getPosition()?
};

/**
 * @brief base class for all objects
 */
class Object {
protected:
	Position p_;
	int32_t dimension_;
	uint32_t weight_;

public:
	// todo: virtual deconstructor
	
	Object(const Position& p, const int32_t dimension, const uint32_t weight) : p_(p), 
		dimension_(dimension), weight_(weight)
	{
	}

	std::pair<int, int> getPosition() const {
		return p_.get();
	}
	
	int32_t getDimension() const {
		return dimension_;
	}
	
	uint32_t getWeight() const {
		return weight_;
	}

	friend std::ostream& operator<< (std::ostream& stream, const Object& o) {
		auto leftTop = o.getPosition();
		std::cout << "[" << leftTop.first << "," << leftTop.second << "|" << 
			leftTop.first + o.getDimension() << "," << leftTop.second + o.getDimension() << "]";

        }
};

class FuelSource : public Object {
public:
	static const int DIMENSION = 2;
	static const uint32_t WEIGHT = 5;
		
	FuelSource(const Position& p) : Object(p, DIMENSION, WEIGHT) {
	}
};

#include "robot.h"

class World {
private:
	/**
	 * @brief helper class that saves state information for each robot to prevent cheating
	 */
	class RobotWithRoundInformation : public Robot {
	private:
		//! automatically cleared by safeUpdate. Set by safeMove() or safeRotate()
		bool activeThisTurn; 
	public:
		RobotWithRoundInformation(const Position& p) : Robot(p), activeThisTurn(false) {
			;
		}

		void safeMove() {
			if(activeThisTurn)
				return;
			; // todo
			activeThisTurn = true;
		}

		void safeRotate(int8_t leftWheel, int8_t rightWheel) {
			if(activeThisTurn)
				return;
			; // todo
			activeThisTurn = true;
		}
		
		void safeUpdate() {
			// todo:
			activeThisTurn = false;
		}
	};
	std::vector<RobotWithRoundInformation> robots_;
	std::unique_ptr<FuelSource> fuelSource_;
	
	const int32_t dimension_;
	
	int doesObjectOverlapWithRobots(const Object& a) const;

public:
	/**
	 * @brief creates an empty world
	 *
	 */
	World(const int32_t dimension) : dimension_(dimension) {
		;
	}
	
	~World() {
	}
	
	/**
	 * adds an robot to {world} at position {p}
	 * 
	 * @return either the unique identifier of that robot or a negative value, in 
	 * case of an error
	 */
	int addRobot(const Position& p);

	/**
	 * @brief returns number of robots currently in this world
	 */
	size_t getNumOfRobots() const {
		return robots_.size();
	}

	/**
	 *
	 */
	std::experimental::optional<Robot> getRobot(int32_t id) const {
		for(auto&& r : robots_) {
			if(r.getID() == id)
				return static_cast<Robot>(r);
		}

		return {};
	}
	
	/**
	 *
	 */
	int addFuelSource(const Position& p);

	/**
	 *
	 */
	FuelSource* getFuelSource() const;

	/**
	 * calculates the tilting angle in Z and Y
	 */
	std::pair<int32_t, int32_t> getWorldPressureVector() const;
	
	uint32_t getDimension() const {
		return dimension_;
	}

	/**
	 * @brief moves the robot
	 */
	int moveRobot(const int32_t robot, bool move);

	/**
	 * @brief rotates the robot by the specified amount of degrees.
	 *
	 * @param leftWheel -90<= degrees <= 90
	 * @param rightWheel -90<= degrees <= 90
	 */
	int rotateRobot(const int32_t robot, const int8_t leftWheel, const int8_t rightWheel);
	
	/**
	 */
	void update();
};

#endif // WORLD_H
