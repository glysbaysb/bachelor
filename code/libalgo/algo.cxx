#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <cassert>

#include "algo.h"

static bool _isInsideCircle(const Vector& position, const double& radius);

static Action _calc_movement(const std::pair<double, double>& angle, const std::vector<Robot>::reverse_iterator& robot,
		const FuelStation& fuel)
{
	Vector accelleration(0., 0.);
#if 0
	/* if this robot might run out of fuel, move towards fuel station */
	if(robot->crit() < CRIT_THRESHHOLD) {
#endif
		accelleration = Vector(100., rotateTowards(robot->pos(), robot->rotation(), fuel.pos()));
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

	
	return Action{robot->id(), unicycle_to_diff(accelleration.x_, accelleration.y_)};
}

static std::vector<Action> _calc_movement(const std::pair<double, double>& angle, const std::vector<Robot>& objects,
		const FuelStation& fuel, const std::vector<Robot>::reverse_iterator& robot)
{
	if(robot == objects.rend()) {
		return {};
	}

	auto action =_calc_movement(angle, robot, fuel);

	// todo: robot->pos() += _actionToVector(action);
	auto xAngle = std::accumulate(objects.begin(), objects.end(), (fuel.pos().x_ * fuel.weight()),
					[](double b, const Robot& a)
					{
						return b + (a.pos().x_ * a.weight());
					});
	auto yAngle = std::accumulate(objects.begin(), objects.end(), (fuel.pos().y_ * fuel.weight()),
					[](double b, const Robot& a)
					{
						return b + (a.pos().y_ * a.weight());
					});

	
	auto actions = std::vector<Action>();
	actions.push_back(action);

	auto otherActions = _calc_movement({xAngle, yAngle}, objects, fuel, robot + 1);
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

static bool _isInsideCircle(const Vector& position, const double& radius)
{
	auto squareDistance = (position.x_ * position.x_) + (position.y_ * position.y_);
	auto squareRadius = radius * radius;

	return squareDistance <= squareRadius;
}

double rotateTowards(const Vector& a, const double rotation, const Vector& b)
{
	auto d = (b - a).norm();
	auto facing = Vector(sin(rotation/ 180 * M_PI), cos(rotation/ 180 * M_PI)).norm();

	auto x = angle(facing, d);

	/* correct the angle, if it's on the left */
	auto dotZ = d.x_ * facing.y_ - d.y_ * facing.x_;
	if(dotZ < 0) {
		x = -x;
	}
	
	return x;
}

Vector get_nearest_point_on_circle(const Vector& pos, const Vector& circleMid, const float radius)
{
	const auto dist = pos - circleMid;
	return circleMid + radius * dist.norm();
}

Vector unicycle_to_diff(const double vel, const double angle)
{
	const auto L = 1.f,
		  R = 0.5f;
	/* v => speed, w => angle
	   v_r = \frac{2v + wL}{2R}
	   v_l = \frac{2v - wL}{2R} */
	auto x = Vector{(2 * vel + angle * L) / 2*R,
		(2 * vel - angle* L) / 2*R
	};
	return x;
}

std::vector<WAYPOINT> gen_path(const unsigned int cnt, const float radius) {
	std::vector<WAYPOINT> points;

	for(auto i = 0.; i < 360; i += (360. / cnt)) {
		auto degInRad = i * M_PI / 180;

		auto x = sin(degInRad);
		auto y = cos(degInRad);
		points.push_back(WAYPOINT{i, x * radius, y * radius});
	}

	return points;
}

WAYPOINT get_nearest_waypoint(const Vector& myPos, const std::vector<WAYPOINT>& path)
{
	auto nearest = WAYPOINT();

	for(auto&& p : path) {
		auto distNearest = Vector(nearest.x_, nearest.y_) - myPos;
		auto distNew = Vector(p.x_, p.y_) - myPos;

		if(distNew < distNearest) {
			nearest = p;
		}
	}

	return nearest;
}

Vector operator-(const WAYPOINT& a, const Vector& b)
{
	auto tmp = Vector(a);
	return Vector(tmp.x_ - b.x_, tmp.y_ - b.y_);
}

static float clamp(float v, float min, float max)
{
	assert(max > min);

	if(v < min) {
		v = min;
	} else if(v > max) {
		v = max;
	}

	return v;
}

float PID(float e, PI& pi)
{
	const float P = 2.f,
		D = 0.01f,
		I = 0.1f;

	timespec tmp;
	clock_gettime(CLOCK_MONOTONIC, &tmp);
	auto timeFrame = (pi.t_prev.tv_sec - tmp.tv_sec) * 1000 +
		lround(pi.t_prev.tv_nsec / 1.0e6 - pi.t_prev.tv_nsec / 1.0e6);
	clock_gettime(CLOCK_MONOTONIC, &pi.t_prev);

	pi.i += e * timeFrame;
	pi.i = clamp(pi.i, -10, 10);
	float deriv = (e - pi.e_prev) / timeFrame;
	pi.e_prev = e;

	return e * P + pi.i * I + deriv * D;
}
