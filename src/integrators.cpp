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
		system.v[i] += h * system.f[i] / system.m[i];
		system.x[i] += h * system.v[i];
	}
	for (size_t i = 0; i < system2.size; ++i)
	{
		system2.v[i] += h * system2.f[i] / system2.m[i];
		system2.x[i] += h * system2.v[i];
		system2.w[i] += h * system2.t[i] / (system2.i[i] * system2.m[i]);
		system2.o[i] += h * system2.w[i];
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
		Vec oldX = system2.x[i];
		system2.x[i] += (h * system2.v[i]) + (h * h * system2.f[i] / system2.m[i]);
		system2.v[i] = (system2.x[i] - oldX) / h;
		unit inertia = system2.i[i] * system2.m[i];
		unit oldO = system2.o[i];
		system2.o[i] += (h * system2.w[i]) + (h * h * system2.t[i] / inertia);
		system2.w[i] = (system2.o[i] - oldO) / h;
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
	Vecs u1 = system2.f;
	auto v1 = system2.t;
	
	saveState();
	subint->integrate(h / 2.0);
	calcForces();
	restoreState();
	Vecs k2 = system.f;
	Vecs u2 = system2.f;
	auto v2 = system2.t;
	
	saveState();
	subint->integrate(h / 2.0);
	calcForces();
	restoreState();
	Vecs k3 = system.f;
	Vecs u3 = system2.f;
	auto v3 = system2.t;
	
	saveState();
	subint->integrate(h);
	calcForces();
	restoreState();
	Vecs k4 = system.f;
	Vecs u4 = system2.f;
	auto v4 = system2.t;
	
	for (size_t i = 0; i < system.size; ++i)
	{
		system.f[i] = k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
		system2.f[i] = u1[i]/6 + u2[i]/3 + u3[i]/3 + u4[i]/6;
		system2.t[i] = v1[i]/6 + v2[i]/3 + v3[i]/3 + v4[i]/6;
	}
	
	subint->integrate(h);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
