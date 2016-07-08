/*******************************************************
 * Vector structure -- header file                     *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *          Henk Alkema                                *
 *                                                     *
 * Date: 15:54 3-6-2016                                *
 *                                                     *
 * Description: Vector structure and operations        *
 *******************************************************/

#ifndef _BASE_H
#define _BASE_H

#include <ostream>

namespace Base {

typedef double unit;

struct Vec2d;
typedef Vec2d Vec; // Default

const unit Pi = 3.1415926535897932384626433832795L;

//------------------------------------------------------------------------------

template <int R>
struct Vector
{
	unit data[R];
	static const int dim = R;
	
	unit &operator[] (int i)
		{ return data[i]; }
};

//------------------------------------------------------------------------------

struct Vec2d : public Vector<2>
{
	unit &x, &y;
	
	Vec2d(unit _x = 0, unit _y = 0) : x(data[0]), y(data[1])
		{ x = _x; y = _y; }
	Vec2d(const Vec2d &v) : x(data[0]), y(data[1]) // Copy constructor
		{ x = v.x; y = v.y; }
	
	Vec2d operator -() const
		{ return Vec2d(-x, -y); }
	Vec2d operator +(unit s) const
		{ return Vec2d(x + s, y + s); }
	Vec2d operator +(const Vec2d &v) const
		{ return Vec2d(x + v.x, y + v.y); }
	Vec2d operator -(unit s) const
		{ return Vec2d(x - s, y - s); }
	Vec2d operator -(const Vec2d &v) const
		{ return Vec2d(x - v.x, y - v.y); }
	Vec2d operator *(unit s) const
		{ return Vec2d(x * s, y * s); }
	Vec2d operator /(unit s) const
		{ return Vec2d(x / s, y / s); }
	
	Vec2d &operator =(const Vec2d &v)
		{ x = v.x; y = v.y; return *this; }
	Vec2d &operator +=(unit s)
		{ x += s; y += s; return *this; }
	Vec2d &operator +=(const Vec2d &v)
		{ x += v.x; y += v.y; return *this; }
	Vec2d &operator -=(unit s)
		{ x -= s; y -= s; return *this; }
	Vec2d &operator -=(const Vec2d &v)
		{ x -= v.x; y -= v.y; return *this; }
	Vec2d &operator *=(unit s)
		{ x *= s; y *= s; return *this; }
	Vec2d &operator /= (unit s)
		{ x /= s; y /= s; return *this; }
	
	unit length() const;
	unit lengthM() const;
	unit length2() const
		{ return x * x + y * y; }
	unit operator *(const Vec2d &v) const // Dot product!
		{ return x * v.x + y * v.y; }
	unit operator &(const Vec2d &v) const // Cross product!
		{ return x * v.y - y * v.x; }
	Vec2d operator ~() const // Normalize!
		{ if (!*this) return Vec2d();
		  unit l = length(); return Vec2d(x / l, y / l); }
	Vec2d rotR() const
		{ return Vec2d(y, -x); }
	Vec2d rotL() const
		{ return Vec2d(-y, x); }
	bool operator !() const // Is zero
		{ return x == 0.0 && y == 0.0; }
	Vec2d operator ^(const Vec2d &v) const
		{ return Vec2d(x * v.x - y * v.y, y * v.x + x * v.y); }
	Vec2d rep() const
		{ return Vec2d(x, -y); } // Assuming v is normalized
		/*{ if (!*this) return Vec2d();
		  unit l2 = length2(); return Vec2d(x / l2, -y / l2); }*/
	unit angle() const;
	
	static Vec2d fromAngle(unit a);
};

static inline Vec2d operator +(unit s, const Vec2d &v)
	{ return Vec2d(s + v.x, s + v.y); }
static inline Vec2d operator -(unit s, const Vec2d &v)
	{ return Vec2d(s - v.x, s - v.y); }
static inline Vec2d operator *(unit s, const Vec2d &v)
	{ return Vec2d(s * v.x, s * v.y); }
static inline Vec2d operator /(unit s, const Vec2d &v)
	{ return Vec2d(s / v.x, s / v.y); }

//------------------------------------------------------------------------------
// Useful for debugging:

template <int R>
std::ostream &operator <<(std::ostream &out, const Vector<R> &v)
{
	out << '(' << v.data[0];
	for (int i = 1; i < R; ++i)
		out << ", " << v.data[i];
	return out << ')';
}

//------------------------------------------------------------------------------

} /* namespace Base */

#endif /* _BASE_H */

//------------------------------------------------------------------------------
