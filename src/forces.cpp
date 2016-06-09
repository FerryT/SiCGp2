/**************************************************************
 * Force definitions -- See header file for more information. *
 **************************************************************/

#include <math.h>

#include "GL/freeglut.h"

#include "forces.h"

namespace Sim {

using namespace Base;

//------------------------------------------------------------------------------

void Spring::draw()
{
	static const double color[3] = {0.6, 0.7, 0.8};
	
	glBegin(GL_LINES);
	glColor3dv(color);
	glVertex2dv(p1.x.data);
	glColor3dv(color);
	glVertex2dv(p2.x.data);
	glEnd();
}

//------------------------------------------------------------------------------

void Spring::apply()
{
	Vec x = p1.x - p2.x,
		v = p1.v - p2.v;
	if (!x)
		return;
	
	unit l = x.length();
	Vec f = (ks * (l - rest) + kd * (v * x) / l) * x / l;
	p1.f += f;
	p2.f -= f;
}

//------------------------------------------------------------------------------

void AngularSpring::draw()
{
	static const double color[3] = {0.6, 0.7, 0.8};
	
	glBegin(GL_LINES);
	glColor3dv(color);
	glVertex2dv(p1.x.data);
	glColor3dv(color);
	glVertex2dv(p2.x.data);
	glColor3dv(color);
	glVertex2dv(p3.x.data);
	glEnd();
}

//------------------------------------------------------------------------------

void AngularSpring::apply()
{
	Vec v1 = p1.x - p2.x,
		v2 = p3.x - p2.x;
	unit cur = (4.0 * Pi) + v1.angle() - v2.angle();
	
	while (cur >= old)
		cur -= (2.0 * Pi);
	if (abs(cur - old) >= abs(cur + (2.0 * Pi) - old))
		cur += (2.0 * Pi);
	old = cur;
	
	double s = -ks * (cur - angle);
	Vec f1 = s * ~v1.rotR();
	Vec f3 = s * ~v1.rotL();
	p1.f += f1;
	p2.f -= (f1 + f3);
	p3.f += f3;
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
