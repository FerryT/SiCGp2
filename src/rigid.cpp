/***********************************************************
 * Rigid entities -- See header file for more information. *
 ***********************************************************/

#include <math.h>

#include "GL/freeglut.h"

#include "effects.h"
#include "rigid.h"
#include "fluid.h"

namespace Sim {

//------------------------------------------------------------------------------

void RigidBox::draw()
{
	RigidBase *rb = this;
	const unit cs = size / 2.0;
	Vec n = Vec::fromAngle(*rb->o);
	Vec c00 =  (Vec(-cs, -cs) ^ n) + *rb->x;
	Vec c01 =  (Vec(-cs,  cs) ^ n) + *rb->x;
	Vec c10 =  (Vec( cs, -cs) ^ n) + *rb->x;
	Vec c11 =  (Vec( cs,  cs) ^ n) + *rb->x;
	if (tex)
	{
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, (GLuint) tex->index);
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
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		static const double color[3] = {0.6, 0.8, 0.7};
		static const double red[3]   = {1.0, 0.0, 0.0};
		static const double green[3] = {0.0, 1.0, 0.0};
		static const double blue[3]  = {0.0, 0.0, 1.0};
		
		glBegin(GL_LINE_LOOP);
		glColor3dv(color);
		glVertex2dv(c00.data);
		glColor3dv(color);
		glVertex2dv(c01.data);
		glColor3dv(color);
		glVertex2dv(c11.data);
		glColor3dv(color);
		glVertex2dv(c10.data);
		glEnd();
		
		glBegin(GL_LINES);
		// Orientation
		glColor3dv(blue);
		glVertex2dv(rb->x->data);
		glVertex2dv((*rb->x + Vec::fromAngle(*rb->o) * 0.1).data);
		// Angular velocity
		//glColor3dv(green);
		//glVertex2dv(rb->x->data);
		//glVertex2dv((*rb->x + Vec::fromAngle(*rb->w) * 0.1).data);
		// Torque
		//glColor3dv(red);
		//glVertex2dv(rb->x->data);
		//glVertex2dv((*rb->x + Vec::fromAngle(*rb->t) * 0.1).data);
		glEnd();
	}
	if (Fluid::VelocityMode)
	{
		static const double cyan[3] = {0.0, 1.0, 1.0};
		glBegin(GL_LINES);
		glColor3dv(cyan);
		glVertex2dv(rb->x->data);
		glVertex2dv((*rb->x + *rb->v).data);
		glEnd();
	}
}

//------------------------------------------------------------------------------

RigidForce::RigidForce(RigidBase *rb, ParticleBase *pb, Vec o)
	: body(rb), p(pb), offset(o)
{
	if (!o && rb)
		offset = *pb->x - *rb->x;
}

//------------------------------------------------------------------------------

void RigidForce::apply()
{
	if (!body) return;
	*p->x = *body->x + (offset ^ Vec::fromAngle(*body->o));
	*p->v = *body->v + (Vec::fromAngle(*body->w) & (*p->x - *body->x));
}

//------------------------------------------------------------------------------

void RigidForce::act(unit h)
{
	if (!body) return;
	*body->f += *p->f;
	*body->t += (*p->x - *body->x) & *p->f;
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
