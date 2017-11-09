#include <vector>
#include <algorithm>
#include <iostream>

#include "algo.h"

static bool _isInsideCircle(const Vector& position, const double& radius)
{
	auto squareDistance = (position.x_ * position.x_) + (position.y_ * position.y_);
	auto squareRadius = radius * radius;

	return squareDistance <= squareRadius;
}

std::vector<Action> calc_movement(const std::pair<double, double>& angle, std::vector<Robot>& objects,
		const FuelStation& fuel)
{
	/* sort the robots by how "critical" their status is - i.e.
	   $ fuel / distance $. In ascending order though ... */
	std::sort(objects.begin(), objects.end());

	/* ... so iterate backwards over the list -> move the robot with
	   the most critical condition first */
	auto actions = std::vector<Action>();
	for(auto it = objects.rbegin(); it != objects.rend(); ++it) {
		Vector accelleration(0., 0.);

		/* if this robot might run out of fuel, move towards fuel station */
		if(it->crit() < CRIT_THRESHHOLD) {
			auto dist = fuel.pos() - it->pos();
			accelleration = Vector(dist.x_ > 0 ? -1. : 1.,
										dist.y_ > 0 ? -1. : 1.);
		}
		/* if it's */
		else if(it->crit() > CRIT_THRESHHOLD && it->crit() < SAFE_THRESHHOLD) {
		}
		/* if it's completly safe, balance agressively */
		else {
			accelleration = Vector(angle.first > 0 ? -1. : 1.,
										angle.second > 0 ? -1. : 1.);
		}

		/* would the robot fall? todo: current acceleration */
		auto expectedPosition = it->pos() + accelleration;

		if(_isInsideCircle(expectedPosition, 50.)) {
			actions.push_back(Action{it->id(), accelleration});
		}
	}

	return actions;
}
