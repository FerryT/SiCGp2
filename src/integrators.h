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

namespace Sim {

using namespace Base;

//------------------------------------------------------------------------------
// All inline on purpose

class Integrator
{
	Integrator(unit _h) : h(_h) {}
	void integrate(Particle &) = 0;

protected:
	unit h;
};

class Euler : public Integrator
{
	Euler(unit h) : Integrator(h) {}
	void integrate(Particle &p)
	{
		p.x += h * p.v;
		p.v += h * p.f / p.m;
	}
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _INTEGRATORS_H */

//..............................................................................
