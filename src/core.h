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

#include "Vector.h"

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
};

//------------------------------------------------------------------------------

/** Classes that can apply forces should inherit this class */
class Appliable
{
public:
	virtual void apply() = 0;
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

} /* namespace Sim */

#endif /* _CORE_H */

//------------------------------------------------------------------------------
