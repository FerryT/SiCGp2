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
	for (size_t i = 0; i < system2.size; ++i)
	{
		unit I = system2.i[i] * system2.m[i]; // Inertia
		system2.x[i] += h * system2.v[i];
		system2.v[i] += h * system2.f[i] / system2.m[i];
		system2.o[i] += h * (system2.w[i] - system2.o[i]);
		system2.w[i] += h * (system2.t[i] - system2.w[i]) / I;
		
		// Normalize orientation and angular velocity
		// Expensive but unfortunately nesessary:
		// without they become unstable due to inaccuracies
		system2.o[i] = ~system2.o[i];
		system2.w[i] = ~system2.w[i];
	}
}

//------------------------------------------------------------------------------

void Verlet::integrate(unit h)
{
	for (size_t i = 0; i < system.size; ++i)
	{
		Vec oldX = system.x[i];
		system.x[i] += (h * system.v[i]) + (h * h * system.f[i] / system.m[i]);
		system.v[i] = (system.x[i] - oldX) / h;
	}
	for (size_t i = 0; i < system2.size; ++i)
	{
		unit I = system2.i[i] * system2.m[i]; // Inertia
		Vec oldX = system2.x[i];
		Vec oldO = system2.o[i];
		system2.x[i] += (h * system2.v[i]) + (h * h * system2.f[i] / system2.m[i]);
		system2.v[i] = (system2.x[i] - oldX) / h;
		system2.o[i] += h * (system2.w[i] - system2.o[i]) + h * h * (system2.t[i] - system2.w[i]) / I;
		system2.w[i] = (system2.o[i] - oldO) / h;
		// Normalize rotations
		system2.o[i] = ~system2.o[i];
		system2.w[i] = ~system2.w[i];
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
	
	for (size_t i = 0; i < system.size; ++i)
		system.f[i] = k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
	
	subint->integrate(h);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
