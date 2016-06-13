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

class Simulation;

//------------------------------------------------------------------------------

class Integrator
{
public:
	Integrator(Simulation &_sim) : sim(_sim) {}
	virtual ~Integrator() {}
	virtual void integrate(unit h) = 0;

protected:
	Simulation &sim;
};

//------------------------------------------------------------------------------

class Euler : public Integrator
{
public:
	Euler(Simulation &sim) : Integrator(sim) {}
	void integrate(unit);
};

//------------------------------------------------------------------------------

class MidPoint : public Integrator
{
public:
	MidPoint(Simulation &sim, Integrator &intg)
		: Integrator(sim), subint(&intg) {}
	void integrate(unit h);

private:
	Integrator *subint;
};

//------------------------------------------------------------------------------

class RungeKutta4 : public Integrator
{
public:
	RungeKutta4(Simulation &sim, Integrator &intg)
		: Integrator(sim), subint(&intg) {}
	void integrate(unit h);
	
private:
	Integrator *subint;
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _INTEGRATORS_H */

//..............................................................................
