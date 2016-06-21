/***********************************************************
 * Rigid entities -- See header file for more information. *
 ***********************************************************/

#include <math.h>

#include "GL/freeglut.h"

#include "effects.h"
#include "rigid.h"

namespace Sim {

//------------------------------------------------------------------------------

void RigidBox::draw()
{
	RigidBase *rb = this;
	const unit cs = size / 2.0;
	Vec c00 =  (Vec(-cs, -cs) ^ *rb->o) + *rb->x;
	Vec c01 =  (Vec(-cs,  cs) ^ *rb->o) + *rb->x;
	Vec c10 =  (Vec( cs, -cs) ^ *rb->o) + *rb->x;
	Vec c11 =  (Vec( cs,  cs) ^ *rb->o) + *rb->x;
	if (tex)
	{
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, (GLuint) tex->index);
	}
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2dv(c00.data);
	glTexCoord2d(0.0, 1.0);
	glVertex2dv(c01.data);
	glTexCoord2d(1.0, 1.0);
	glVertex2dv(c11.data);
	glTexCoord2d(1.0, 0.0);
	glVertex2dv(c10.data);
	glEnd();
	glFlush();
	if (tex)
		glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

void RigidParticle::apply()
{
	if (!body) return;
	ParticleBase *pb = this;
	*body->f = *pb->f;
	unit r = (*pb->f - ((s * *pb->f) / s)).length() * l;
	*body->t = Vec(cos(r), sin(r));
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
