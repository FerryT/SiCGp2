/*******************************************************
 * Rigid entities -- header file                       *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *                                                     *
 * Date: 21:06 15-6-2016                               *
 *                                                     *
 * Description: Rigid and indeformable body entity     *
 *              classes.                               *
 *******************************************************/

#ifndef _RIGID_H
#define _RIGID_H

#include "core.h"
#include "effects.h"

namespace Sim {

//------------------------------------------------------------------------------

class RigidBase : public Entity
{
public:
	Vec *x, *v, *f;
	Vec *o, *w, *t;
	unit *m;

	RigidBase(Vec *_x, Vec *_v, Vec *_f, Vec *_o, Vec *_w, Vec *_t, unit *_m)
		: x(_x), v(_v), f(_f), o(_o), w(_w), t(_t), m(_m) {}
	virtual ~RigidBase() {}
	virtual unit body() { return 1.0; }
};

//------------------------------------------------------------------------------

class RigidBody : public RigidBase
{
public:
	Vec x, v, f;
	Vec o, w, t; // Orientation, angular velocity, torque
	unit m;

	RigidBody(Vec _x = Vec(), Vec _o = Vec(1.0, 0.0), unit _m = 1.0)
		: RigidBase(&x, &v, &f, &o, &w, &t, &m), x(_x), o(_o), m(_m) {}
	virtual ~RigidBody() {}
};

//------------------------------------------------------------------------------

struct RigidSystem
{
	Vecs x, v, f, o, w, t, i;
	units m;
	size_t size;
	RigidSystem() : x(), v(), f(), o(), w(), t(), m(), i(), size(0) {}
};

//------------------------------------------------------------------------------

class RigidBox : public RigidBody, virtual public Drawable
{
public:
	
	unit size;
	Texture *tex;
	
	RigidBox(unit _size, Vec x, Vec o = Vec(1.0, 0.0), unit m = 1.0, Texture *_tex = 0)
		: RigidBody(x, o, m), size(_size), tex(_tex) {}
	
	void draw();
	virtual unit body() { return (size * size) / 6.0; }
};

//------------------------------------------------------------------------------

class RigidParticle : public Particle, virtual public Appliable
{
public:
	RigidBase *body;
	Vec offset;

	RigidParticle(RigidBase *rb = NULL, Vec o = Vec())
		: body(rb), offset(o), s(~o), l(o.length()) {}

	void apply();

private:
	Vec s;
	unit l;
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _RIGID_H */

//..............................................................................
