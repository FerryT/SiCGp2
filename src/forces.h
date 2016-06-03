/*******************************************************
 * Force definitions -- header file                    *
 *                                                     *
 * Authors: Henk Alkema                                *
 *          Ferry Timmers                              *
 *                                                     *
 * Date: 18:41 3-6-2016                                *
 *                                                     *
 * Description: Forces that can influence particles in *
 *              a particle system.                     *
 *******************************************************/

#ifndef _FORCES_H
#define _FORCES_H

#include "core.h"

namespace Sim {

using namespace Base;

//------------------------------------------------------------------------------

class Force : public virtual Drawable, public virtual Appliable
{
public:
	virtual void draw() {}
	virtual void apply() {}
};

//------------------------------------------------------------------------------

class Gravity : public Force
{
public:
	
};

//------------------------------------------------------------------------------

class Spring : public Force
{
public:
	Particle &p1, &p2;
	unit rest;
	const unit &ks, &kd;
	
	Spring(Particle &_p1, Particle &_p2, unit _rest, const unit &_ks,
		const unit &_kd) : p1(_p1), p2(_p2), rest(_rest), ks(_ks), kd(_kd) {}
	
	void draw();
	void apply();
};

//------------------------------------------------------------------------------

class AngularSpring : public Force
{
public:
	Particle &p1, &p2, &p3;
	unit angle;
	const unit &ks;
	
	AngularSpring(Particle &_p1, Particle &_p2, Particle &_p3, unit _angle,
		const unit &_ks) : p1(_p1), p2(_p2), p3(_p3), angle(_angle), ks(_ks),
		old(Pi / 2) {}
	
	virtual void draw();
	virtual void apply();

private:
	unit old;
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _FORCES_H */

//..............................................................................
