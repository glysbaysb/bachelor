#ifndef CONTROLLER_STRUCTS_H
#define CONTROLLER_STRUCTS_H

struct WorldStatus
{
	float xTilt_,
		  yTilt_;
	std::vector<Object> objects;

	friend std::ostream& operator<<(std::ostream& os, const WorldStatus& ws) {
		os << ws.xTilt_ << ';' << ws.yTilt_ << '\n';
		for(auto&& i : ws.objects) {
			os << i;
		}

		return os;
	}

};
#endif // CONTROLLER_STRUCTS_H

