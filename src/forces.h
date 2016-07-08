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
#include "sim.h"

namespace Sim {

using namespace Base;

class Simulation;

//------------------------------------------------------------------------------

class Force : public Entity, public virtual Appliable
{
public:
	virtual void apply() {}
};

//------------------------------------------------------------------------------

class Gravity : public Force, public virtual Drawable
{
public:
	Simulation *sim;
	Vec g, origin;
	
	Gravity(Simulation *_sim, Vec _g, Vec _origin = Vec(.5,.5))
		: sim(_sim), g(_g), origin(_origin) {}
	
	void draw();
	void apply();
};

//------------------------------------------------------------------------------

class Spring : public Force, public virtual Drawable
{
public:
	ParticleBase *p1, *p2;
	unit rest;
	unit ks, kd;
	
	Spring(ParticleBase *_p1, ParticleBase *_p2, unit _rest, const unit &_ks,
		const unit &_kd) : p1(_p1), p2(_p2), rest(_rest), ks(_ks), kd(_kd) {}
	
	virtual void draw();
	virtual void apply();
};

//------------------------------------------------------------------------------

class AngularSpring : public Force, public virtual Drawable
{
public:
	ParticleBase *p1, *p2, *p3;
	unit angle;
	const unit ks;
	
	AngularSpring(ParticleBase *_p1, ParticleBase *_p2, ParticleBase *_p3,
		unit _angle, const unit &_ks)
		: p1(_p1), p2(_p2), p3(_p3), angle(_angle), ks(_ks), old(Pi / 2) {}
	
	virtual void draw();
	virtual void apply();

private:
	unit old;
};

//------------------------------------------------------------------------------

class Glue : public Force
{
public:
	ParticleBase *p;
	Vec x;
	
	Glue(ParticleBase *_p, Vec _x) : p(_p), x(_x) {}
	
	virtual void apply();
};

//------------------------------------------------------------------------------

class Borders : public Force
{
public:
	Simulation *sim;
	unit absorbtion;
	
	Borders(Simulation *_sim, unit a = 0.5) : sim(_sim), absorbtion(a) {}
	
	virtual void apply();
};

//------------------------------------------------------------------------------

/* *** Experimental *** */
class Collisions : public Force
{
public:
	Simulation *sim;
	
	Collisions(Simulation *_sim) : sim(_sim) {}
	
	virtual void apply();
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _FORCES_H */

//..............................................................................
