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

} /* namespace Sim */

//..............................................................................
