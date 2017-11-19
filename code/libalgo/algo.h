#ifndef ALGO_H
#define ALGO_H

#include <cfloat>
#include <cmath>

#define CRIT_THRESHHOLD 20
#define SAFE_THRESHHOLD 80

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
	Object(int id, Vector pos, double rot) :
		id_(id), pos_(pos), rotation_(rot)
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

	Robot(int id, Vector pos, double rotation, double fuel) :
		Object(id, pos, rotation), fuel_(fuel)
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
	FuelStation(int id, Vector pos, double rotation) :
		Object(id, pos, rotation)
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
