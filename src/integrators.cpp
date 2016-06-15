/**********************************************************************
 * Integration functionality -- See header file for more information. *
 **********************************************************************/

#include "integrators.h"

namespace Sim {

//------------------------------------------------------------------------------

void Euler::integrate(unit h)
{
	for (size_t i = 0; i < system.size; ++i)
	{
		system.x[i] += h * system.v[i];
		system.v[i] += h * system.f[i] / system.m[i];
	}
}

//------------------------------------------------------------------------------

void MidPointBase::integrate(unit h)
{
	saveState();
	subint->integrate(h / 2.0);
	calcForces();
	restoreState();
	subint->integrate(h);
}

//------------------------------------------------------------------------------

void RungeKutta4Base::integrate(unit h)
{
	Vecs k1 = system.f;
	
	saveState();
	subint->integrate(h / 2.0);
	calcForces();
	restoreState();
	Vecs k2 = system.f;
	
	saveState();
	subint->integrate(h / 2.0);
	calcForces();
	restoreState();
	Vecs k3 = system.f;
	
	saveState();
	subint->integrate(h);
	calcForces();
	restoreState();
	Vecs k4 = system.f;
	
	for (size_t i = system.size - 1; i >= 0; --i)
		system.f[i] = k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
	
	subint->integrate(h);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
