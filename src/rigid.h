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

namespace Sim {

//------------------------------------------------------------------------------

class RigidBody : public Entity, virtual public Drawable
{
public:
	virtual ~RigidBody() {}
};

//------------------------------------------------------------------------------

class RigidBox : public RigidBody
{
public:
	Vec x, v, f;
	Vec o, w, t; // Orientation, angular velocity, torque
	unit m;
	unit size;
	
	RigidBox(unit _size, Vec _x, Vec _o = Vec(0.0, 1.0), unit _m = 1.0)
		: size(_size), x(_x), o(_o), m(_m) {}
	
	void draw();
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _RIGID_H */

//..............................................................................
