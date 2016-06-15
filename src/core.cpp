/*************************************************************
 * Entity structure -- See header file for more information. *
 *************************************************************/

#include "GL/freeglut.h"

#include "core.h"

namespace Sim {

//------------------------------------------------------------------------------

void ParticleBase::draw()
{
	static const double h = 0.005;
	static const double white[3] = {1.0, 1.0, 1.0};
	
	glColor3dv(white);
	
	glBegin(GL_QUADS);
	glVertex2d(x->x - h, x->y - h);
	glVertex2d(x->x + h, x->y - h);
	glVertex2d(x->x + h, x->y + h);
	glVertex2d(x->x - h, x->y + h);
	glEnd();
}

//------------------------------------------------------------------------------

std::ostream &operator <<(std::ostream &out, const ParticleBase &p)
{
	return out << "P< " << *p.x << ", " << *p.v
		<< ", " << *p.f << ", " << *p.m << " >";
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//------------------------------------------------------------------------------
