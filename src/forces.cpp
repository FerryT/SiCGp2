/**************************************************************
 * Force definitions -- See header file for more information. *
 **************************************************************/

#include <math.h>

#include "GL/freeglut.h"

#include "forces.h"

namespace Sim {

using namespace Base;

//------------------------------------------------------------------------------

template <int N, int M>
unit SAT_axis(Vec A[N], Vec B[M], Vec axis)
{
	unit amin = 1.0 / 0.0;
	unit amax = -1.0 / 0.0;
	for (int i = 0; i < N; ++i)
	{
		unit a = axis * A[i];
		if (a < amin) amin = a;
		if (a > amax) amax = a;
	}
	unit bmin = 1.0 / 0.0;
	unit bmax = -1.0 / 0.0;
	for (int i = 0; i < M; ++i)
	{
		unit b = axis * B[i];
		if (b < bmin) bmin = b;
		if (b > bmax) bmax = b;
	}
	unit min = amin > bmin ? amin : bmin;
	unit max = amax < bmax ? amax : bmax;
	return max - min;
}

template <int N, int M>
std::pair<Vec,unit> SAT(Vec A[N], Vec B[M])
{
	unit overlap = 1.0 / 0.0;
	Vec dir;
	for (int i = 0; i < N; ++i)
	{
		Vec u = A[i];
		Vec v = A[(i + 1) % N];
		Vec axis = ~(v - u).rotR();
		unit o = SAT_axis<N,M>(A, B, axis);
		if (o < overlap)
		{
			dir = -axis;
			overlap = o;
		}
	}
	for (int i = 0; i < M; ++i)
	{
		Vec u = B[i];
		Vec v = B[(i + 1) % M];
		Vec axis = ~(u - v).rotR();
		unit o = SAT_axis<N,M>(A, B, axis);
		if (o < overlap)
		{
			dir = axis;
			overlap = o;
		}
	}
	return std::pair<Vec,unit>(dir, overlap);
}

void makePolygon(Vec polygon[], const Quad &q)
{
	polygon[0] = *q.p1->x;
	polygon[1] = *q.p2->x;
	polygon[2] = *q.p3->x;
	polygon[3] = *q.p4->x;
}

void makePolygon(Vec polygon[], const RigidBase &r)
{
	const RigidBox *rb = dynamic_cast<const RigidBox *> (&r);
	unit cs = 1.0;
	if (rb) cs = rb->size / 2.0;
	Vec n = Vec::fromAngle(*r.o);
	polygon[0] = (Vec(-cs, -cs) ^ n) + *r.x;
	polygon[1] = (Vec(-cs,  cs) ^ n) + *r.x;
	polygon[2] = (Vec( cs,  cs) ^ n) + *r.x;
	polygon[3] = (Vec( cs, -cs) ^ n) + *r.x;
}

template <typename T, typename U>
std::pair<Vec,unit> SAT(const T &a, const U &b)
{
	const Vec nan = Vec(0.0 / 0.0, 0.0 / 0.0);
	Vec A[4] = {nan,nan,nan,nan};
	Vec B[4] = {nan,nan,nan,nan};
	makePolygon(A, a);
	makePolygon(B, b);
	return SAT<4,4>(A, B);
}

//------------------------------------------------------------------------------

void Gravity::draw()
{
	Vec pos = origin + g;
	Vec arrow = (g + g.rotL()) / 4.0;
	glBegin(GL_LINES);
	glColor3f(0.8, 0.7, 0.6);
	glVertex2dv(origin.data);
	glColor3f(0.8, 0.7, 0.6);
	glVertex2dv(pos.data);
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.8, 0.7, 0.6);
	glVertex2dv(pos.data);
	glColor3f(0.8, 0.7, 0.6);
	glVertex2d(pos.x + arrow.x, pos.y - arrow.y);
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.8, 0.7, 0.6);
	glVertex2dv(pos.data);
	glColor3f(0.8, 0.7, 0.6);
	glVertex2dv((pos - arrow).data);
	glEnd();
}

//------------------------------------------------------------------------------

void Gravity::apply()
{
	for (ParticleBase **p = sim->getParticles(); *p; ++p)
		*(**p).f += *(**p).m * g;
	for (RigidBase **r = sim->getRigids(); *r; ++r)
	{
		Vec P[4];
		makePolygon(P, **r);
		Vec f = *(**r).m * g;
		*(**r).f += f;
		for (int i = 0; i < 4; ++i)
		{
			if (P[i].y < (*r)->x->y)
				*(**r).t += (P[i] - *(**r).x) & f;
		}
	}
}

//------------------------------------------------------------------------------

void Spring::draw()
{
	unit l = fabs((*p1->x - *p2->x).length() - rest);
	const double color[3] = {0.6 + l, 0.7, 0.8 - l};
	
	glBegin(GL_LINES);
	glColor3dv(color);
	glVertex2dv(p1->x->data);
	glColor3dv(color);
	glVertex2dv(p2->x->data);
	glEnd();
}

//------------------------------------------------------------------------------

void Spring::apply()
{
	Vec x = *p1->x - *p2->x,
		v = *p1->v - *p2->v;
	if (!x)
		return;
	
	unit l = x.length();
	Vec f = ((ks * (l - rest)) + (kd * (v * x) / l)) * (x / l);
	
	*p1->f += f;
	*p2->f -= f;
}

//------------------------------------------------------------------------------

void AngularSpring::draw()
{
	static const double color[3] = {0.6, 0.7, 0.8};
	
	glBegin(GL_LINES);
	glColor3dv(color);
	glVertex2dv(p1->x->data);
	glColor3dv(color);
	glVertex2dv(p2->x->data);
	glColor3dv(color);
	glVertex2dv(p3->x->data);
	glEnd();
}

//------------------------------------------------------------------------------

void AngularSpring::apply()
{
	Vec v1 = *p1->x - *p2->x,
		v2 = *p3->x - *p2->x;
	unit cur = (4.0 * Pi) + v1.angle() - v2.angle();
	
	while (cur >= old)
		cur -= (2.0 * Pi);
	if (abs(cur - old) >= abs(cur + (2.0 * Pi) - old))
		cur += (2.0 * Pi);
	old = cur;
	
	double s = -ks * (cur - angle);
	Vec f1 = s * ~v1.rotR();
	Vec f3 = s * ~v1.rotL();
	*p1->f += f1;
	*p2->f -= (f1 + f3);
	*p3->f += f3;
}

//------------------------------------------------------------------------------

void Glue::apply()
{
	*p->x = x;
	*p->v = Vec();
	*p->f = Vec();
}

//------------------------------------------------------------------------------

void Borders::apply()
{
	for (ParticleBase **p = sim->getParticles(); *p; ++p)
	{
		if ((*p)->x->x < sim->bounds.left)
		{
			(*p)->x->x = sim->bounds.left;
			(*p)->v->x *= -absorbtion;
			(*p)->f->x = 0;
		}
		if ((*p)->x->x > sim->bounds.right)
		{
			(*p)->x->x = sim->bounds.right;
			(*p)->v->x *= -absorbtion;
			(*p)->f->x = 0;
		}
		if ((*p)->x->y > sim->bounds.bottom)
		{
			(*p)->x->y = sim->bounds.bottom;
			(*p)->v->y *= -absorbtion;
			(*p)->f->y = 0;
		}
		if ((*p)->x->y < sim->bounds.top)
		{
			(*p)->x->y = sim->bounds.top;
			(*p)->v->y *= -absorbtion;
			(*p)->f->y = 0;
		}
	}
	for (RigidBase **r = sim->getRigids(); *r; ++r)
	{
		Vec P[4];
		makePolygon(P, **r);
		for (int i = 0; i < 4; ++i)
		{
			Vec p = P[i];
			if (p.x < sim->bounds.left)
			{
				Vec f = Vec(sim->bounds.left - p.x, 0);
				*(*r)->x += f;
				*(*r)->v *= -absorbtion;
				*(*r)->f = 0;
				*(*r)->w *= -absorbtion;
				*(*r)->t = 0;
			}
			if (p.x > sim->bounds.right)
			{
				Vec f = Vec(sim->bounds.right - p.x, 0);
				*(*r)->x += f;
				*(*r)->v *= -absorbtion;
				*(*r)->f = 0;
				*(*r)->w *= -absorbtion;
				*(*r)->t = 0;
			}
			if (p.y > sim->bounds.bottom)
			{
				Vec f = Vec(0, sim->bounds.bottom - p.y);
				*(*r)->x += f;
				*(*r)->v *= -absorbtion;
				*(*r)->f = 0;
				*(*r)->w *= -absorbtion;
				*(*r)->t = 0;
			}
			if (p.y < sim->bounds.top)
			{
				Vec f = Vec(0, sim->bounds.top - p.y);
				*(*r)->x += f;
				*(*r)->v *= -absorbtion;
				*(*r)->f = 0;
				*(*r)->w *= -absorbtion;
				*(*r)->t = 0;
			}
		}
	}
}

//------------------------------------------------------------------------------

Vec getCenter(Quad &q) { return q.center(); }
Vec getCenter(RigidBase &r) { return *r.x; }

Vec getVel(Quad &q, Vec x) { return q.velocity(); }
Vec getVel(RigidBase &r, Vec x)
{
	return *r.v + (Vec::fromAngle(*r.w) & (x - *r.x));
}

void setForce(Quad &q, Vec f, Vec x)
{
	*q.p1->v = f;
	*q.p2->v = f;
	*q.p3->v = f;
	*q.p4->v = f;
}
void setForce(RigidBase &r, Vec f, Vec x)
{
	*r.v = f;
	*r.w = (x - *r.x) & f;
}

void correctPos(Quad &q, Vec v)
{
	*q.p1->x += v;
	*q.p2->x += v;
	*q.p3->x += v;
	*q.p4->x += v;
}
void correctPos(RigidBase &r, Vec v)
{
	*r.v -= v;
	*r.x -= v;
}

template <typename T, typename U>
void collide(Collisions &col, T &a, U &b)
{
	const unit absorbtion = 0.6;
	const unit rest = 0.2;
	std::pair<Vec,unit> overlap = SAT(a, b);
	if (overlap.second <= 0.0) return;
	
	Vec n = overlap.first;
	unit d = overlap.second;
	Vec x = n * d * 0.5;
	Vec n1 = getCenter(b) - getCenter(a);
	Vec n2 = getCenter(a) - getCenter(b);
	Vec v1 = getVel(a, x);
	Vec v2 = getVel(b, x);
	Vec f1 = (-v1 * (1.0 - rest) + ((n1 * v2) >= 0.0 ? -v2 : v2) * rest) * absorbtion;
	Vec f2 = (-v2 * (1.0 - rest) + ((n2 * v1) >= 0.0 ? -v1 : v1) * rest) * absorbtion;
	Vec pv = n * d * 0.2;
	if (d*d > 0.00001)
	{
		correctPos(a, -pv);
		correctPos(b, pv);
	}
	setForce(a, f1, -x);
	setForce(b, f2, x);
}

void Collisions::apply()
{
	for (Quad **q1 = sim->getQuads(); *q1; ++q1)
		for (Quad **q2 = q1 + 1; *q2; ++q2)
			collide(*this, **q1, **q2);
	for (Quad **q = sim->getQuads(); *q; ++q)
		for (RigidBase **r = sim->getRigids(); *r; ++r)
			collide(*this, **q, **r);
	for (RigidBase **r1 = sim->getRigids(); *r1; ++r1)
		for (RigidBase **r2 = r1 + 1; *r2; ++r2)
			collide(*this, **r1, **r2);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
