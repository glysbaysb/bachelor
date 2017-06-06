#ifndef ROBOT_H
#define ROBOT_H

class Robot : public Object {
private:
	int8_t fuelStatus_; //!< 0 <= fuelStatus_ <= 100
	
	int32_t id_;
	static int32_t GLOBAL_ID; //!< 0 at programm startup, incremented for each invocation of constructor

	Position movementVector_;

protected:
	void move(const Position& diff) {
		/* calc new vector */
		auto newMovement = movementVector_ + diff;
		/* if it doesn't speed up too much, set the new speed */
		if(abs(newMovement.get().first) <= 3 &&
		   abs(newMovement.get().second) <= 3)
		{
			movementVector_ = newMovement;
		}
	}	

	void setFuelStatus(const int8_t fuelStatus) {
		assert(fuelStatus >= 0 && fuelStatus <= 100);
		fuelStatus_ = fuelStatus;
	}

	void update() {
		std::cout << p_;
		std::cout << " becomes " << (p_ + movementVector_) << '\n';
		p_ = p_ + movementVector_;
		// todo: fuel
	}

public:
	static const uint32_t WEIGHT = 1;
	static const int DIMENSION = 3;

	Robot(const Position& p) : Object(p, DIMENSION, WEIGHT), id_(GLOBAL_ID++),
		movementVector_({0, 0})
	{
		setFuelStatus(100);
	}
	
	int8_t getFuelStatus() const {
		return fuelStatus_;
	}
	
	uint32_t getWeight() const {
		// todo: somehow make this dependent on fuel status
		return weight_;
	}


	int32_t getID() const {
		return id_;
	}

	Position getMovementVector() const {
		return movementVector_;
	}
};

#endif
