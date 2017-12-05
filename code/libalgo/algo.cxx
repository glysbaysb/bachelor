#include <vector>
#include <algorithm>
#include <iostream>
#include <map>

#include "algo.h"

static bool _isInsideCircle(const Vector& position, const double& radius)
{
	auto squareDistance = (position.x_ * position.x_) + (position.y_ * position.y_);
	auto squareRadius = radius * radius;

	return squareDistance <= squareRadius;
}

static double _rotateTowards(const Vector& a, const double rotation, const Vector& b)
{
	auto d = (a - b).norm();
	auto facing = Vector(sin(rotation/ 180 * M_PI), cos(rotation/ 180 * M_PI));

	auto x = 180 - angle(facing, d);

	/* correct the angle, if it's on the left */
	auto dotZ = d.y_ * facing.x_ - d.x_ * facing.y_;
	if(dotZ < 0) {
		x = -x;
	}
	
	std::cout << "winkel: " << x << '\n';
	return x;
}

static Action _calc_movement(const std::pair<double, double>& angle, const std::vector<Robot>::reverse_iterator& robot,
		const FuelStation& fuel)
{
	Vector accelleration(0., 0.);
#if 0
	/* if this robot might run out of fuel, move towards fuel station */
	if(robot->crit() < CRIT_THRESHHOLD) {
#endif
		accelleration = Vector(1., _rotateTowards(robot->pos(), robot->rotation(), fuel.pos()));
#if 0
	}
	/* if it's */
	else if(robot->crit() > CRIT_THRESHHOLD && robot->crit() < SAFE_THRESHHOLD) {
	}
	/* if it's completly safe, balance agressively */
	else {
		accelleration = Vector(1.,
									angle.second > 0 ? -1. : 1.);
	}
#endif
	/* todo: would the robot fall? todo: current acceleration */
	return Action{robot->id(), accelleration};
}

static std::vector<Action> _calc_movement(const std::pair<double, double>& angle, const std::vector<Robot>& objects,
		const FuelStation& fuel, const std::vector<Robot>::reverse_iterator& robot)
{
	if(robot == objects.rend()) {
		return {};
	}

	auto actions = std::vector<Action>();
	actions.push_back(_calc_movement(angle, robot, fuel));

	/* todo: update angle */
	
	auto otherActions = _calc_movement(angle, objects, fuel, robot + 1);
	actions.insert(std::end(actions), std::begin(otherActions), std::end(otherActions));
	return actions;
}
 
std::vector<Action> calc_movement(const std::pair<double, double>& angle, std::vector<Robot>& objects,
		const FuelStation& fuel)
{
	/* sort the robots by how "critical" their status is - i.e.
	   $ fuel / distance $. In ascending order though ... */
	std::sort(objects.begin(), objects.end());

	/* ... so start the recursion with the last element -> move the robot with
	   the most critical condition first */
	return _calc_movement(angle, objects, fuel, objects.rbegin());
}
