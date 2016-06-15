/*************************************************************
 * Fluid simulation -- See header file for more information. *
 *************************************************************/

#include "GL/freeglut.h"

#include "fluid.h"

namespace Sim {

#define IX(i,j) ((i)+(width+2)*(j))
#define SWAP(x0,x) {unit *tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL(i,j) \
	for (int i = 1; i <= width; ++i) { \
	for (int j = 1; j <= height; ++j) {
#define END_FOR }}

//------------------------------------------------------------------------------

Fluid::Fluid(Simulation *s, int w, int h, unit V, unit D, Vec G)
	: sim(s), width(w), height(h), visc(V), diff(D), g(G)
{
	const size_t size = (w + 2)*(h + 2);
	u     = new unit[size];
	u_old = new unit[size];
	v     = new unit[size];
	v_old = new unit[size];
	d     = new unit[size];
	d_old = new unit[size];

	std::fill(u, u + size, 0.0);
	std::fill(u_old, u_old + size, 0.0);
	std::fill(v, v + size, 0.0);
	std::fill(v_old, v_old + size, 0.0);
	std::fill(d, d + size, 0.0);
	std::fill(d_old, d_old + size, 0.0);
}

Fluid::~Fluid()
{
	delete[] u;
	delete[] u_old;
	delete[] v;
	delete[] v_old;
	delete[] d;
	delete[] d_old;
}

//------------------------------------------------------------------------------

void Fluid::draw()
{
	unit x, y, d00, d01, d10, d11;
	unit r = (unit) width / (unit) height,
		dx = (sim->bounds.right - sim->bounds.left) / (unit) width,
		dy = (sim->bounds.bottom - sim->bounds.top) / (unit) height;
	
	glBegin(GL_QUADS);
	for (int i = 0; i <= width; ++i)
	{
		x = (i - 0.5) * dx + sim->bounds.left;
		for (int j = 0; j <= height; ++j)
		{
			y = (j - 0.5) * dy + sim->bounds.top;
			d00 = d[IX(i, j)];
			d01 = d[IX(i, j + 1)];
			d10 = d[IX(i + 1, j)];
			d11 = d[IX(i + 1, j + 1)];
			glColor3d(0, d00/2, d00);
			glVertex2d(x, y);
			glColor3d(0, d10/2, d10);
			glVertex2d(x + dx, y);
			glColor3d(0, d11/2, d11);
			glVertex2d(x + dx, y + dy);
			glColor3d(0, d01/2, d01);
			glVertex2d(x, y + dy);
		}
	}
	glEnd();
}

void Fluid::act(unit dt)
{
	const size_t size = (width + 2)*(height + 2);
	
	// Reset, apply gravity
	for (int i = 0; i < size; ++i)
	{
		u_old[i] = 0.0;
		v_old[i] = 0.0;
		d_old[i] = 0.0;
	}
	FOR_EACH_CELL(i, j)
		if (i > 1 && i < width - 1 && j > 1 && j < height - 1)
		{
			u_old[IX(i, j)] = g.x * d[IX(i, j)] * 1.0;
			v_old[IX(i, j)] = g.y * d[IX(i, j)] * 1.0;
		}
	END_FOR
	// Debug:
	u_old[IX(width/2 + 1, height - 5)] = 500.0;
	v_old[IX(width/2 + 1, height - 5)] = 0.0;
	d_old[IX(width/2 + 1, height - 5)] = 300.0;
	vel_step(u, v, u_old, v_old, visc, dt);
	dens_step(d, d_old, u, v, diff, dt);
}

//------------------------------------------------------------------------------

void Fluid::add_source(unit *x, unit *s, unit dt)
{
	int size = (width + 2) * (height + 2);
	for (int i = 0; i < size; ++i)
		x[i] += dt * s[i];
}

void Fluid::set_bnd(int b, unit *x)
{
	const int &W = width;
	const int &H = height;
	
	for (int i = 1; i <= W; ++i)
	{
		x[IX(i, 0  )] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, H+1)] = b == 2 ? -x[IX(i, H)] : x[IX(i, H)];
	}
	for (int j = 1; j <= H; ++j)
	{
		x[IX(0  , j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
		x[IX(W+1, j)] = b == 1 ? -x[IX(W, j)] : x[IX(W, j)];
	}
	
	x[IX(0  , 0  )] = 0.5 * (x[IX(1, 0  )] + x[IX(0  , 1)]);
	x[IX(0  , H+1)] = 0.5 * (x[IX(1, H+1)] + x[IX(0  , H)]);
	x[IX(W+1, 0  )] = 0.5 * (x[IX(W, 0  )] + x[IX(W+1, 1)]);
	x[IX(W+1, H+1)] = 0.5 * (x[IX(W, H+1)] + x[IX(W+1, H)]);
}

void Fluid::lin_solve(int b, unit *x, unit *x0, unit a, unit c)
{
	for (int k = 0 ; k < 20 ; k++)
	{
		FOR_EACH_CELL(i, j)
			x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)]
				+ x[IX(i, j - 1)] + x[IX(i, j + 1)])) / c;
		END_FOR
		set_bnd(b, x);
	}
}

void Fluid::diffuse(int b, unit *x, unit *x0, unit diff, unit dt)
{
	unit a = dt * diff * width * height;
	lin_solve(b, x, x0, a, 1 + 4 * a);
}

void Fluid::advect(int b, unit *d, unit *d0, unit *u, unit *v, unit dt)
{
	int i0, j0, i1, j1;
	unit x, y, s0, t0, s1, t1, dt0;
	dt0 = dt * width;
	FOR_EACH_CELL(i, j)
		x = i - dt0 * u[IX(i, j)];
		y = j - dt0 * v[IX(i, j)];
		
		if (x < 0.5) x = 0.5;
		if (x > width + 0.5) x = width + 0.5;
		
		i0 = (int) x;
		i1 = i0 + 1;
		
		if (y < 0.5) y = 0.5;
		if (y > height + 0.5) y = height + 0.5;
		
		j0 = (int) y;
		j1 = j0 + 1;
		s1 = x - i0;
		s0 = 1 - s1;
		t1 = y - j0;
		t0 = 1 - t1;
		d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
		              s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
	END_FOR
	set_bnd(b, d);
}

void Fluid::project(unit *u, unit *v, unit *p, unit *div)
{
	FOR_EACH_CELL(i, j)
		div[IX(i, j)] = -0.5 * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]) / height;
		p[IX(i, j)] = 0;
	END_FOR
	set_bnd(0, div);
	set_bnd(0, p);
	lin_solve(0, p, div, 1, 4);
	FOR_EACH_CELL(i, j)
		u[IX(i, j)] -= 0.5 * width * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
		v[IX(i, j)] -= 0.5 * height * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
	END_FOR
	set_bnd(1, u);
	set_bnd(2, v);
}

void Fluid::dens_step(unit *x, unit *x0, unit *u, unit *v, unit diff, unit dt)
{
	add_source(x, x0, dt);
	SWAP(x0, x);
	diffuse(0, x, x0, diff, dt);
	SWAP(x0, x);
	advect(0, x, x0, u, v, dt);
}

void Fluid::vel_step(unit *u, unit *v, unit *u0, unit *v0, unit visc, unit dt)
{
	add_source(u, u0, dt);
	add_source(v, v0, dt);
	SWAP(u0, u);
	diffuse(1, u, u0, visc, dt);
	SWAP(v0, v);
	diffuse(2, v, v0, visc, dt);
	project(u, v, u0, v0);
	SWAP(u0, u);
	SWAP(v0, v);
	advect(1, u, u0, u0, v0, dt);
	advect(2, v, v0, u0, v0, dt);
	project(u, v, u0, v0);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
