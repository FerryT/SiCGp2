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

#include <ostream>

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

class Particle : public virtual Entity, public virtual Drawable
{
public:
	Vec x, v, f;
	unit m;
	
	Particle(Vec _x = Vec(), Vec _v = Vec(), Vec _f = Vec(), unit _m = 0.0) :
		x(_x), v(_v), f(_f), m(_m) {}
	
	virtual void draw();
};

std::ostream &operator <<(std::ostream &out, const Particle &);

//------------------------------------------------------------------------------

class ParticleSystem
{
public:
	Particle *particles;
	const size_t count;
	
	ParticleSystem() : count(0), particles((void *) 0) {}
	ParticleSystem(size_t count);
	ParticleSystem(const ParticleSystem &);
	ParticleSystem(ParticleSystem &&);
	~ParticleSystem();
	
	void operator =(const ParticleSystem &) = delete;
	
	void copy(const ParticleSystem &);
	void copyForces(const ParticleSystem &);
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _CORE_H */

//------------------------------------------------------------------------------
