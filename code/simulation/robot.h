#ifndef ROBOT_H
#define ROBOT_H

class Robot : public Object {
private:
	int fuelStatus_; //!< 0 <= fuelStatus_ <= 100
	
	int32_t id_;
	static int32_t GLOBAL_ID; //!< 0 at programm startup, incremented for each invocation of constructor

	Position movementVector_;
protected:
	/**
	 * @brief updates the movement vector by diff
	 *
	 * However this function limits it to a maximum speed of 3 in each
	 * direction.
	 * todo: limit speed increase to 1 per turn
	 */
	void move(const Position& diff) {
		/* calc new vector */
		auto newmovement = movementVector_ + diff;
		/* if it doesn't speed up too much, set the new speed */
		if(abs(newmovement._x) <= 3 &&
		   abs(newmovement._y) <= 3)
		{
			movementVector_ = newmovement;
		}
	}	

	void setFuelStatus(const int fuelStatus) {
		/* clamp */
		if(fuelStatus > 100)
			fuelStatus_ = 100;
		else if(fuelStatus < 0)
			fuelStatus_ = 0;
		else
			fuelStatus_ = fuelStatus;
	}

	void update() {
		// todo: is that one simulation cycle too late?
		setFuelStatus(getFuelStatus() - 1); // always use up a bit

		/* is there enoguh fuel to move that much */
		if(fuelStatus_ < abs(movementVector_)) {
			/* todo: slow down */
			return;
		}

		p_ = p_ + movementVector_;

		/* fuel */
		auto speed = abs(movementVector_);
		setFuelStatus(getFuelStatus() - floor(speed));

	}

public:
	static const uint32_t WEIGHT = 1;
	static const int DIMENSION = 3;
	
	Robot(const Position& p) : Object(p, DIMENSION, WEIGHT), id_(GLOBAL_ID++),
		movementVector_({0, 0}), fuelStatus_(100)
	{
	}
	
	int getFuelStatus() const {
		return fuelStatus_;
	}
	
	uint32_t getWeight() const {
		return weight_ + floor(fuelStatus_ * 0.03);
	}


	int32_t getID() const {
		return id_;
	}

	Position getMovementVector() const {
		return movementVector_;
	}
};

#endif
