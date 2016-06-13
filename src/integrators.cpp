/**********************************************************************
 * Integration functionality -- See header file for more information. *
 **********************************************************************/

#include "integrators.h"

namespace Sim {

//------------------------------------------------------------------------------

void Euler::integrate(unit h)
{
	for (size_t i = 0; i < sim.system.size; ++i)
	{
		sim.system.x[i] += sim.system.v[i] * h;
		sim.system.v[i] += sim.system.f[i] * h / sim.system.m[i];
	}
}

//------------------------------------------------------------------------------

void MidPoint::integrate(unit h)
{
	sim.saveState();
	subint->integrate(h / 2.0);
	sim.calcForces();
	sim.restoreState();
	subint->integrate(h);
}

//------------------------------------------------------------------------------

void RungeKutta4::integrate(unit h)
{
	Vecs k1 = sim.system.f;
	
	sim.saveState();
	subint->integrate(h / 2.0);
	sim.calcForces();
	sim.restoreState();
	Vecs k2 = sim.system.f;
	
	sim.saveState();
	subint->integrate(h / 2.0);
	sim.calcForces();
	sim.restoreState();
	Vecs k3 = sim.system.f;
	
	sim.saveState();
	subint->integrate(h);
	sim.calcForces();
	sim.restoreState();
	Vecs k4 = sim.system.f;
	
	for (size_t i = sim.system.size - 1; i >= 0; --i)
		sim.system.f[i] = k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
	
	subint->integrate(h);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
