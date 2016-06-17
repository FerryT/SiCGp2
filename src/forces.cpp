/**************************************************************
 * Force definitions -- See header file for more information. *
 **************************************************************/

#include <math.h>

#include "GL/freeglut.h"

#include "forces.h"

namespace Sim {

using namespace Base;

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
			(*p)->v->x *= -1;
			(*p)->f->x = 0;
		}
		if ((*p)->x->x > sim->bounds.right)
		{
			(*p)->x->x = sim->bounds.right;
			(*p)->v->x *= -1;
			(*p)->f->x = 0;
		}
		if ((*p)->x->y > sim->bounds.bottom)
		{
			(*p)->x->y = sim->bounds.bottom;
			(*p)->v->y *= -1;
			(*p)->f->y = 0;
		}
		if ((*p)->x->y < sim->bounds.top)
		{
			(*p)->x->y = sim->bounds.top;
			(*p)->v->y *= -1;
			(*p)->f->y = 0;
		}
	}
}

//------------------------------------------------------------------------------

bool collides(const Quad &q, const ParticleBase &p)
{
	unit c1 = (*q.p2->x - *q.p1->x) * (*p.x - *q.p1->x);
	unit c2 = (*q.p3->x - *q.p2->x) * (*p.x - *q.p2->x);
	unit c3 = (*q.p4->x - *q.p3->x) * (*p.x - *q.p3->x);
	unit c4 = (*q.p1->x - *q.p4->x) * (*p.x - *q.p4->x);
	//if (!c1 || !c2 || !c3 || !c4)
	//	return true;
	
	int c = 0;
	c += (c1 < 0) ? -1 : 1;
	c += (c2 < 0) ? -1 : 1;
	c += (c3 < 0) ? -1 : 1;
	c += (c4 < 0) ? -1 : 1;
	return (c == 4) || (c == -4);
}

inline Vec project(Vec u, Vec v, Vec p)
{
	Vec uv = v - u;
	unit t = (uv * (p - u)) / uv.length2();
	if (t > 1.0) t = 1.0;
	if (t < 0.0) t = 0.0;
	return u + t * uv;
}

Vec closest(const Quad &q, const ParticleBase &p)
{
	Vec p1 = project(*q.p1->x, *q.p2->x, *p.x);
	Vec p2 = project(*q.p2->x, *q.p3->x, *p.x);
	Vec p3 = project(*q.p3->x, *q.p4->x, *p.x);
	Vec p4 = project(*q.p4->x, *q.p1->x, *p.x);
	unit d1 = (*p.x - p1).length2();
	unit d2 = (*p.x - p2).length2();
	unit d3 = (*p.x - p3).length2();
	unit d4 = (*p.x - p4).length2();
	if (d1 <= d2 && d1 <= d3 && d1 <= d4) return p1;
	if (d2 <= d1 && d2 <= d3 && d2 <= d4) return p2;
	if (d3 <= d1 && d3 <= d2 && d3 <= d4) return p3;
	if (d4 <= d1 && d4 <= d2 && d4 <= d3) return p4;
	return p1;
}

void Collisions::apply()
{
	for (Quad **q = sim->getQuads(); *q; ++q)
	{
		for (ParticleBase **p = sim->getParticles(); *p; ++p)
		{
			if (*p == (*q)->p1 || *p == (*q)->p2
			|| *p == (*q)->p3 || *p == (*q)->p4)
				continue;
			
			if (collides(**q, **p))
			{
				Vec pc = closest(**q, **p);
				Vec n = ~(*(**p).x - pc);
				Vec v = *(**p).v;
				if (v * n < 0)
					continue;
				*(**p).x = pc;
				*(**q).p1->v += *(**p).v / 4.0;
				*(**q).p2->v += *(**p).v / 4.0;
				*(**q).p3->v += *(**p).v / 4.0;
				*(**q).p4->v += *(**p).v / 4.0;
				*(**p).v = 0.0;//(v - 2.0 * (v * n) * n);
				*(**p).f = 0.0;
			}
		}
	}
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
