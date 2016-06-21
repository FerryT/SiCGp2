/*******************************************************
 * Integration functionality -- header file            *
 *                                                     *
 * Authors: Henk Alkema                                *
 * Ferry Timmers                                       *
 *                                                     *
 * Date: 1:18 7-6-2016                                 *
 *                                                     *
 * Description: Different integration metods used by   *
 *              the simulation                         *
 *******************************************************/

#ifndef _INTEGRATORS_H
#define _INTEGRATORS_H

#include "base.h"
#include "core.h"
#include "sim.h"

namespace Sim {

using namespace Base;

//------------------------------------------------------------------------------

class Integrator
{
public:
	Integrator(Simulation &_sim)
		: sim(_sim), system(_sim.getSystem()), system2(_sim.getSystem2()) {}
	virtual ~Integrator() {}
	virtual void integrate(unit h) = 0;

protected:
	Simulation &sim;
	ParticleSystem &system;
	RigidSystem &system2;
	
	void saveState() { sim.saveState(); }
	void restoreState() { sim.restoreState(); }
	void calcForces() { sim.calcForces(); }
};

//------------------------------------------------------------------------------

class Euler : public Integrator
{
public:
	Euler(Simulation &sim) : Integrator(sim) {}
	void integrate(unit);
};

//------------------------------------------------------------------------------

class Verlet : public Integrator
{
public:
	Verlet(Simulation &sim) : Integrator(sim) {}
	void integrate(unit);
};

//------------------------------------------------------------------------------

class MidPointBase : public Integrator
{
public:
	MidPointBase(Simulation &sim, Integrator *i) : Integrator(sim), subint(i) {}
	virtual ~MidPointBase() {}
	void integrate(unit h);
protected:
	Integrator *subint;
};

template <class I> class MidPoint : public MidPointBase
{
public:
	MidPoint(Simulation &sim) : MidPointBase(sim, new I(sim)) {}
	~MidPoint() { delete subint; }
};

//------------------------------------------------------------------------------

class RungeKutta4Base : public Integrator
{
public:
	RungeKutta4Base(Simulation &sim, Integrator *i) : Integrator(sim), subint(i) {}
	virtual ~RungeKutta4Base() {}
	void integrate(unit h);
protected:
	Integrator *subint;
};

template <class I> class RungeKutta4 : public RungeKutta4Base
{
public:
	RungeKutta4(Simulation &sim) : RungeKutta4Base(sim, new I(sim)) {}
	~RungeKutta4() { delete subint; }
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _INTEGRATORS_H */

//..............................................................................
