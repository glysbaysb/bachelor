/**
 * @file The algorithm that determines how each robot should move considers how
 * critical the status of each robot it. "critical" is defined as fuel left divided
 * by the distance to the fuel station.
 *
 * There are three possible cases:
 * - Either the value is above @CRIT_THRESHHOLD, then the robot may try to balance the
 *   plate
 * - Or it is belove @SAFE_THRESHHOLD then the robot has to move towards the
 *   fuel station ASAP
 * - Lastely it may be somewhere in between. In that case it also tries to balance the
 *   plate
 *
 * The algorithm firstly calculates how the most critical robot should move, then the
 * second most and so on. After each robot the algorithm calculates how the world
 * should look after each robot has moved. Then the algorithm takes that into
 * account when calculating the next movements
 */
#ifndef ALGO_H
#define ALGO_H

#include <cfloat>
#include <cmath>

#define CRIT_THRESHHOLD 20
#define SAFE_THRESHHOLD 80

class Action;

struct Vector
{
	double x_,
		  y_;

	Vector(double x, double y) :
		x_(x), y_(y)
	{
	}

	Vector operator+(const Vector& a) const
	{
		return Vector(x_ + a.x_, y_ + a.y_);
	}

	Vector& operator+=(const Vector& a)
	{
		x_ += a.x_;
		y_ += a.y_;

		return *this;
	}
	
	Vector operator-(const Vector& a) const
	{
		return Vector(x_ - a.x_, y_ - a.y_);
	}

	double length() const
	{
		return sqrt(x_ * x_ + y_ * y_);
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector& p) {
		return os << '(' << p.x_ << ';' << p.y_ << ')';
	}

	friend bool operator==(const Vector& a, const Vector& b) {
		return ((a.x_ - b.x_) < FLT_EPSILON) && ((a.y_ - b.y_) < FLT_EPSILON);
	}

	friend double angle(const Vector& a, const Vector& b) {
		auto tmp = (a.x_* b.x_ + a.y_ * b.y_);
		return acos(tmp / (a.length() * b.length())) * 180 / M_PI;
	}

	friend bool operator<(const Vector& a, const Vector& b) {
		return a.length() < b.length();
	}

	Vector norm() {
		auto len = length();
		return Vector{x_ / len, y_ / len};
	}
};

class Object
{
protected:
	Vector pos_;
	double rotation_;
	double m_;
	int id_;
	//todo: Vector velocity_;

public:
	Object(int id, Vector pos, double rot, double m) :
		id_(id), pos_(pos), rotation_(rot), m_(m)
	{
	}

	Vector pos() const
	{
		return pos_;
	}

	int id() const
	{
		return id_;
	}

	friend std::ostream& operator<<(std::ostream& os, const Object& o) {
		return os << '#' << o.id_ << '\n' <<
			'\t' << o.pos_ << '\n' <<
			'\t' << o.rotation_ << '\n';
	}

	double rotation() const
	{
		return rotation_;
	}

	double weight() const
	{
		return m_;
	}
};

class Robot : public Object
{
private:
	double fuel_;
public:
	double crit() const
	{
		return fuel_ / pos().length();
	}

	bool operator<(const Robot& a) const
	{
		return crit() < a.crit();
	}

	Robot(int id, Vector pos, double rotation, double fuel, double m) :
		Object(id, pos, rotation, m), fuel_(fuel)
	{
	}

	friend std::ostream& operator<<(std::ostream& os, const Robot& o) {
		return os << '#' << o.id_ << '\n' <<
			'\t' << o.pos_ << '\n' <<
			'\t' << o.rotation_ << '\n' <<
			'\t' << o.fuel_ << '\n' <<
			'\t' << o.crit() << '\n';
	}
};

class FuelStation: public Object
{
public:
	FuelStation(int id, Vector pos, double rotation, double m) :
		Object(id, pos, rotation, m)
	{
	}
};

class Action
{
private:
	int id_;
	Vector acceleration_;

public:
	Action(int id, Vector acceleration) :
		id_(id), acceleration_(acceleration)
	{
	}

	int id() const { return id_; }
	Vector acceleration() const { return acceleration_; }

	friend std::ostream& operator<<(std::ostream& os, const Action& o) {
		return os << '#' << o.id_ << '+' << o.acceleration_ << '\n';
	}

	friend bool operator<(const Action& a, const Action& b) {
		return a.id_ < b.id_ &&
			a.acceleration_.length() < b.acceleration_.length();
	}
};

std::vector<Action> calc_movement(const std::pair<double, double>& angle, std::vector<Robot>& objects,
		const FuelStation& fuel);

#endif // ALGO_H
