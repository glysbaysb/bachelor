#ifndef ROBOT_H
#define ROBOT_H

class Robot : public Object {
private:
	int8_t fuelStatus_; // 0 <= fuelStatus_ <= 100
	
	int32_t id_;
	static int32_t GLOBAL_ID; //! 0 at programm startup, incremented for each invocation of constructor

protected:
	void move() {
		assert(false);
	}	

	void setFuelStatus(const int8_t fuelStatus) {
		assert(fuelStatus >= 0 && fuelStatus <= 100);
		fuelStatus_ = fuelStatus;
	}
public:
	static const uint32_t WEIGHT = 1;
	static const int DIMENSION = 3;

	Robot(const Position& p) : Object(p, DIMENSION, WEIGHT), id_(GLOBAL_ID++) {
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
};

#endif
