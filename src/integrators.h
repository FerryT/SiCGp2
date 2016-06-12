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
	Integrator(unit _h) : h(_h) {}
	virtual ~Integrator() {}
	virtual void integrate(Simulation &_sim) = 0;

protected:
	unit h;
};

class Euler : public Integrator
{
	Euler(unit h) : Integrator(h) {}
	void integrate(Simulation &_sim);
};

class Midpoint : public Integrator
{
	Midpoint(unit h) : Integrator(h) {}
	void integrate(Simulation &_sim);
};

class RungeKutta4 : public Integrator
{
	RungeKutta4(unit h) : Integrator(h) {}
	void integrate(Simulation &_sim);
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _INTEGRATORS_H */

//..............................................................................
