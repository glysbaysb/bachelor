#ifndef CONTROLLER_STRUCTS_H
#define CONTROLLER_STRUCTS_H

struct WorldStatus
{
	float xTilt_,
		  yTilt_;
	std::vector<Robot> robots;
	FuelStation* fuelStation;//fixme: mem leak

	friend std::ostream& operator<<(std::ostream& os, const WorldStatus& ws) {
		os << ws.xTilt_ << ';' << ws.yTilt_ << '\n';
		for(auto&& i : ws.robots) {
			os << i;
		}
		os << *ws.fuelStation;

		return os;
	}

};
#endif // CONTROLLER_STRUCTS_H

