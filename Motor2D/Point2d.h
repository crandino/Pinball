//=================================
// include guard
#ifndef __Point2d_H__
#define __Point2d_H__

template <class TYPE>
class Point2d {

public:

	TYPE x, y;

	Point2d()
	{ }

	Point2d(const Point2d &p)
	{
		x = p.x;
		y = p.y;
	}

	Point2d(const TYPE &px, const TYPE &py)
	{
		x = px;
		y = py;
	}

	Point2d operator+ (const Point2d &p) const
	{
		Point2d tmp;
		tmp.x = x + p.x;
		tmp.y = y + p.y;
		return tmp;
	}

	Point2d operator- (const Point2d &p) const
	{
		Point2d tmp;
		tmp.x = x - p.x;
		tmp.y = y - p.y;
		return tmp;
	}

	const Point2d& operator+= (const Point2d &p)
	{
		x += p.x;
		y += p.y;
		return (*this);
	}

	const Point2d& operator-= (const Point2d &p)
	{
		x -= p.x;
		y -= p.y;
		return (*this);
	}

	bool operator== (const Point2d &p) const
	{
		return (x == p.x && y == p.y);
	}

	bool operator!= (const Point2d &p) const
	{
		return (x != p.x || y != p.y);
	}

	bool isBothZero() const
	{
		return (x == 0.0f && y == 0.0f);
	}

	void setZero()
	{
		x = y = 0;
	}

	TYPE distanceTo(const Point2d &p) const
	{
		TYPE dx = x - p.x;
		TYPE dy = y - p.y;
		return sqrt(dx*dx + dy*dy);
	}

	bool isClosedTo(const Point2d &p, float delta) const
	{
		return (abs(p.x - x) <= delta && abs(p.y - y) <= delta);
	}

};

typedef Point2d<int> iPoint;

template<class TYPE>
class Vector2d : public Point2d<TYPE>
{
public:

	Vector2d() : Point2d()
	{}

	Vector2d(const Point2d& p) : Point2d(p)
	{}

	Vector2d(TYPE x, TYPE y) : Point2d(x, y)
	{}

	void normalize()
	{
		float m = x * x + y * y;
		if (m > 0.0f)
		{
			float inv_m = 1 / sqrtf(m);
			x *= inv_m;
			y *= inv_m;
		}
	}

	float length() const
	{
		return sqrtf(float((x*x) + (y*y)));
	}

	Vector2d operator -(const TYPE v) const
	{
		Vector2d<TYPE> r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	Vector2d& operator*=(float a)
	{
		x *= a;
		y *= a;
		return(*this);
	}

	Vector2d& operator*(float a)
	{
		Vector2d<TYPE> r;

		r.x = x * a;
		r.y = y * a;

		return r;
	}

	Vector2d& operator/=(float a)
	{
		float f = 1.0f / a;
		x *= f;
		y *= f;
		return(*this);
	}

	TYPE dot(const Vector2d<TYPE>& v) const
	{
		return(x * v.x + y * v.y);
	}
};

typedef Vector2d<int> iVector;
typedef Vector2d<float> fVector;

#endif // !__Point2d_H__
