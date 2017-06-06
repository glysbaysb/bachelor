/**
 * @file
 * @section DESCRIPTION
 * Definitions for a few auxiliary classes and, most importantly, the world.
 *
 *
 *
 */
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
	
	friend Position operator+ (const Position& a, const Position& b) {
		return Position(a._x + b._x, a._y + b._y);
        }
	
	friend std::ostream& operator<< (std::ostream& stream, const Position& o) {
		std::cout << "[" << o._x << "," << o._y << "]";

        }

	friend double abs(const Position& a) {
		return sqrt(a._x * a._x + a._y * a._y);
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
		auto leftTop = o.p_;
		auto rightBottom = leftTop + Position{o.getDimension(), o.getDimension()};
		std::cout << leftTop << "," << rightBottom;

        }
};

class FuelSource : public Object {
private:
	static const int DIMENSION = 2;
	static const uint32_t WEIGHT = 5;
public:
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

		void safeMove(const Position& diff) {
			if(activeThisTurn)
				return;
			move(diff);
			activeThisTurn = true;
		}

		void safeUpdate() {
			update();
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
	 * @brief returns a *copy* of the specified robot.
	 *
	 * That copy can be used to gather all kinds of information about the robot
	 * but returns potentially invalid information after the next call to
	 * World::update()
	 *
	 * @param id the ID of the robot
	 * @return either a optional, so that it is possible to check whether 
	 * a robot was found or not. The caller is supposed to check for that
	 * before using the robot
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
	int moveRobot(const int32_t robot, const Position& diffVector);

	/**
	 * @brief actually commits the changes that were set by ::moveRobot() 
	 */
	void update();
};

#endif // WORLD_H
