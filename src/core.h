/*******************************************************
 * Entity structure -- header file                     *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *                                                     *
 * Date: 17:39 3-6-2016                                *
 *                                                     *
 * Description: Base classes for all perceivable       *
 *              entities.                              *
 *******************************************************/

#ifndef _CORE_H
#define _CORE_H

#include <stdlib.h>
#include <vector>
#include <iostream>

#include "base.h"

namespace Sim {

using namespace Base;

//------------------------------------------------------------------------------

class Entity
{
public:
	virtual ~Entity() {} // Makes entity a virtual class
};

//------------------------------------------------------------------------------

/** Classes that need to be drawn should inherit this class */
class Drawable
{
public:
	virtual void draw() = 0; // Makes Drawable an abstract class
	virtual ~Drawable() {}
};

//------------------------------------------------------------------------------

/** Classes that can apply forces should inherit this class */
class Appliable
{
public:
	virtual void apply() = 0;
	virtual ~Appliable() {}
};

//------------------------------------------------------------------------------

/** Classes that can act in teh simulation */
class Actor
{
public:
	virtual void act(unit dt) = 0;
	virtual ~Actor() {};
};

//------------------------------------------------------------------------------

class ParticleBase : public Entity, public virtual Drawable
{
public:
	Vec *x, *v, *f;
	unit *m;
	
	ParticleBase(Vec *_x = 0, Vec *_v = 0, Vec *_f = 0, unit *_m = 0) :
		x(_x), v(_v), f(_f), m(_m) {}
	
	virtual void draw();
};

std::ostream &operator <<(std::ostream &out, const ParticleBase &);

//------------------------------------------------------------------------------

class Particle : public ParticleBase
{
public:
	Vec x, v, f;
	unit m;
	
	Particle(Vec _x = Vec(), Vec _v = Vec(), Vec _f = Vec(), unit _m = 1.0) :
		ParticleBase(&x, &v, &f, &m), x(_x), v(_v), f(_f), m(_m) {}
};

//------------------------------------------------------------------------------

typedef std::vector<Vec> Vecs;
typedef std::vector<unit> units;

struct ParticleSystem
{
	Vecs x, v, f;
	units m;
	size_t size;
	ParticleSystem() : x(), v(), f(), m(), size(0) {}
};

//------------------------------------------------------------------------------

class Quad : public Entity
{
public:
	ParticleBase *p1, *p2, *p3, *p4;
	Quad(ParticleBase *_p1, ParticleBase *_p2, ParticleBase *_p3, ParticleBase *_p4)
		: p1(_p1), p2(_p2), p3(_p3), p4(_p4) {}
	
	bool collides(const Vec &p) const;
	Vec closest(const Vec &p) const;
	//Vec project(const Vec &v) const;
	//std::pair<Vec,unit> penetration(const Quad &q) const; // Penetration vector
	Vec center() const
		{ return (*p1->x + *p2->x + *p3->x + *p4->x) / 4.0; }
	Vec velocity() const
		{ return (*p1->v + *p2->v + *p3->v + *p4->v) / 4.0; }
	unit mass() const
		{ return (*p1->m + *p2->m + *p3->m + *p4->m) / 4.0; }
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _CORE_H */

//------------------------------------------------------------------------------
