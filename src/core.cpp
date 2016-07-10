/*************************************************************
 * Entity structure -- See header file for more information. *
 *************************************************************/

#include "GL/freeglut.h"

#include "core.h"
#include "fluid.h"

namespace Sim {

//------------------------------------------------------------------------------

void ParticleBase::draw()
{
	static const double h = 0.003;
	static const double white[3] = {1.0, 1.0, 1.0};
	static const double cyan[3] = {0.0, 1.0, 1.0};
	
	glColor3dv(white);
	
	glBegin(GL_QUADS);
	glVertex2d(x->x - h, x->y - h);
	glVertex2d(x->x + h, x->y - h);
	glVertex2d(x->x + h, x->y + h);
	glVertex2d(x->x - h, x->y + h);
	glEnd();
	
	if (Fluid::VelocityMode)
	{
		glColor3dv(cyan);
		
		glBegin(GL_LINES);
		glVertex2dv(x->data);
		glVertex2dv((*x + *v).data);
		glEnd();
	}
}

//------------------------------------------------------------------------------

std::ostream &operator <<(std::ostream &out, const ParticleBase &p)
{
	return out << "P< " << *p.x << ", " << *p.v
		<< ", " << *p.f << ", " << *p.m << " >";
}

//------------------------------------------------------------------------------

bool Quad::collides(const Vec &p) const
{
	unit c1 = (*p2->x - *p1->x) * (p - *p1->x);
	unit c2 = (*p3->x - *p2->x) * (p - *p2->x);
	unit c3 = (*p4->x - *p3->x) * (p - *p3->x);
	unit c4 = (*p1->x - *p4->x) * (p - *p4->x);
	
	int c = 0;
	c += (c1 < 0.0) ? -1 : 1;
	c += (c2 < 0.0) ? -1 : 1;
	c += (c3 < 0.0) ? -1 : 1;
	c += (c4 < 0.0) ? -1 : 1;
	return (c == 4) || (c == -4);
}
/*
Vec Quad::project(const Vec &v) const
{
	unit d1 = *p1->x * v;
	unit d2 = *p2->x * v;
	unit d3 = *p3->x * v;
	unit d4 = *p4->x * v;
	if (d1 >= d2 && d1 >= d3 && d1 >= d4) return *p1->x;
	if (d2 >= d1 && d2 >= d3 && d2 >= d4) return *p2->x;
	if (d3 >= d1 && d3 >= d2 && d3 >= d4) return *p3->x;
	if (d4 >= d1 && d4 >= d2 && d4 >= d3) return *p4->x;
}

std::pair<Vec,unit> Quad::penetration(const Quad &q) const
{
	Vec n1 = ~(*p2->x - *p1->x).rotR();
	Vec n2 = ~(*p3->x - *p2->x).rotR();
	Vec n3 = ~(*p4->x - *p3->x).rotR();
	Vec n4 = ~(*p1->x - *p4->x).rotR();
	unit d1 = n1 * (q.project(-n1) - *p1->x);
	unit d2 = n2 * (q.project(-n2) - *p2->x);
	unit d3 = n3 * (q.project(-n3) - *p3->x);
	unit d4 = n4 * (q.project(-n4) - *p4->x);
	if (d1 >= d2 && d1 >= d3 && d1 >= d4) return std::pair<Vec,unit>(n1, d1);
	if (d2 >= d1 && d2 >= d3 && d2 >= d4) return std::pair<Vec,unit>(n2, d2);
	if (d3 >= d1 && d3 >= d2 && d3 >= d4) return std::pair<Vec,unit>(n3, d3);
	if (d4 >= d1 && d4 >= d2 && d4 >= d3) return std::pair<Vec,unit>(n4, d4);
}
*/
//------------------------------------------------------------------------------

inline Vec _project(Vec u, Vec v, Vec p)
{
	Vec uv = v - u;
	unit t = (uv * (p - u)) / uv.length2();
	if (t > 1.0) t = 1.0;
	if (t < 0.0) t = 0.0;
	return u + t * uv;
}

Vec Quad::closest(const Vec &p) const
{
	Vec x1 = _project(*p1->x, *p2->x, p);
	Vec x2 = _project(*p2->x, *p3->x, p);
	Vec x3 = _project(*p3->x, *p4->x, p);
	Vec x4 = _project(*p4->x, *p1->x, p);
	unit d1 = (p - x1).length2();
	unit d2 = (p - x2).length2();
	unit d3 = (p - x3).length2();
	unit d4 = (p - x4).length2();
	if (d1 >= d2 && d1 <= d3 && d1 <= d4) return x1;
	if (d2 >= d1 && d2 <= d3 && d2 <= d4) return x2;
	if (d3 >= d1 && d3 <= d2 && d3 <= d4) return x3;
	if (d4 >= d1 && d4 <= d2 && d4 <= d3) return x4;
	return x1;
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//------------------------------------------------------------------------------
