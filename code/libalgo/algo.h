#ifndef ALGO_H
#define ALGO_H

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

	friend std::ostream& operator<<(std::ostream& os, const Vector& p) {
		return os << '(' << p.x_ << ';' << p.y_ << ')';
	}
};

class Object
{
protected:
	Vector pos_;
	double m_;
	int id_;
	//todo: Vector velocity_;

public:
	Object(int id, Vector pos) :
		id_(id), pos_(pos)
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
			'\t' << o.pos_ << '\n';
	}
};

class Robot : public Object
{
private:
	double fuel_;
public:
	double crit() const
	{
		return fuel_ / (sqrt(pos().x_ * pos().x_ + pos().y_ * pos().y_));
	}

	bool operator<(const Robot& a) const
	{
		return crit() < a.crit();
	}
};

class FuelStation: public Object
{
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
};

std::vector<Action> calc_movement(const std::pair<double, double>& angle, std::vector<Robot>& objects,
		const FuelStation& fuel);

#endif // ALGO_H
