/*******************************************************
 * Fluid simulation -- header file                     *
 *                                                     *
 * Based on prototype by Jos Stam, 2003                *
 *                                                     *
 * Date: 10:46 15-6-2016                               *
 *                                                     *
 *******************************************************/

#ifndef _FLUID_H
#define _FLUID_H

#include "base.h"
#include "core.h"
#include "sim.h"

namespace Sim {

//------------------------------------------------------------------------------

class Fluid : public Entity, virtual public Drawable, virtual public Actor
{
public:
	Simulation *sim;
	const int width, height;
	unit visc, diff;
	Vec g;
	unit *u, *u_old; // velocity x
	unit *v, *v_old; // velocity y
	unit *d, *d_old; // density
	struct
	{
		Vec pos;
		unit d;
		Vec v;
	} mouse;
	
	Fluid(Simulation *sim, int width, int height, unit visc = 0.0, unit diff = 0.0, Vec g = Vec());
	~Fluid();
	
	void draw();
	void act(unit dt);

private:
	void add_source(unit *x, unit *s, unit dt);
	void set_bnd(int b, unit *x);
	void lin_solve(int b, unit *x, unit *x0, unit a, unit c);
	void diffuse(int b, unit *x, unit *x0, unit diff, unit dt);
	void advect(int b, unit *d, unit *d0, unit *u, unit *v, unit dt);
	void project(unit *u, unit *v, unit *p, unit *div);
	void dens_step(unit *x, unit *x0, unit *u, unit *v, unit diff, unit dt);
	void vel_step(unit *u, unit *v, unit *u0, unit *v0, unit visc, unit dt);
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _FLUID_H */

//..............................................................................
