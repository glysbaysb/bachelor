#include <vector>
#include <algorithm>

#include "algo.h"

std::vector<Action> calc_movement(std::pair<float, float>& angle, std::vector<Robot>& objects,
		FuelStation& fuel)
{
	/* sort the robots by how "critical" their status is - i.e.
	   $ fuel / distance $. In ascending order though ... */
	std::sort(objects.begin(), objects.end());

	/* ... so iterate backwards over the list -> move the robot with
	   the most critical condition first */
	auto actions = std::vector<Action>();
	for(auto it = objects.rbegin(); it != objects.rend(); ++it) {
		/* if this robot might run out of fuel, move towards fuel station */
		if(it->crit() < CRIT_THRESHHOLD) {
			auto dist = fuel.pos() - it->pos();
			auto accelleration = Vector{dist.x_ > 0 ? -1. : 1.,
										dist.y_ > 0 ? -1. : 1.};
			actions.push_back(Action{it->id(), accelleration});
		}
		/* if it's */
		else if(it->crit() > CRIT_THRESHHOLD && it->crit() < SAFE_THRESHHOLD) {
		}
		/* if it's completly safe, balance agressively */
		else {
		}
	}

	return actions;
}
