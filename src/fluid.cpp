/*************************************************************
 * Fluid simulation -- See header file for more information. *
 *************************************************************/

#include "math.h"
#include <queue>
#include <algorithm>

#include "GL/freeglut.h"

#include "fluid.h"

namespace Sim {

#define IX(i,j) ((i)+(width+2)*(j))
#define CLAMPX(i) ((i) < 0 ? 0 : (i) > (width+1) ? (width+1) : (i))
#define CLAMPY(j) ((j) < 0 ? 0 : (j) > (height+1) ? (height+1) : (j))
#define SWAP(x0,x) {unit *tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL(i,j) \
	for (int i = 1; i <= width; ++i) { \
	for (int j = 1; j <= height; ++j) {
#define END_FOR }}

bool Fluid::VelocityMode = false;

//------------------------------------------------------------------------------

Fluid::Fluid(Simulation *s, int w, int h, unit V, unit D, Vec G, unit S)
	: sim(s), width(w), height(h), visc(V), diff(D), g(G), speed(S)
{
	const size_t size = (w + 2)*(h + 2);
	u     = new unit[size];
	u_old = new unit[size];
	v     = new unit[size];
	v_old = new unit[size];
	d     = new unit[size];
	d_old = new unit[size];
	p = new Entity *[size];

	std::fill(u, u + size, 0.0);
	std::fill(u_old, u_old + size, 0.0);
	std::fill(v, v + size, 0.0);
	std::fill(v_old, v_old + size, 0.0);
	std::fill(d, d + size, 0.0);
	std::fill(d_old, d_old + size, 0.0);
	std::fill(p, p + size, nullptr);
}

Fluid::~Fluid()
{
	delete[] u;
	delete[] u_old;
	delete[] v;
	delete[] v_old;
	delete[] d;
	delete[] d_old;
	delete[] p;
}

//------------------------------------------------------------------------------

void Fluid::draw()
{
	unit x, y, d00, d01, d10, d11;
	unit r = (unit) width / (unit) height,
		dx = (sim->bounds.right - sim->bounds.left) / (unit) width,
		dy = (sim->bounds.bottom - sim->bounds.top) / (unit) height;
	
	if (Fluid::VelocityMode)
	{
		glBegin(GL_LINES);
		for (int i = 0; i <= width; ++i)
		{
			x = (i - 0.5) * dx + sim->bounds.left;
			for (int j = 0; j <= height; ++j)
			{
				y = (j - 0.5) * dy + sim->bounds.top;
				Vec vel(u[IX(i, j)], v[IX(i, j)]);
				unit norm = vel.length();
				vel /= norm;
				vel = Vec(vel.x * dx, vel.y * dy);
				glColor3d(norm / 4.0, 0.0, 1.0 - norm / 2.0);
				glVertex2d(x, y);
				glColor3d(1.0, 1.0, 1.0);
				glVertex2d(x + vel.x, y + vel.y);
			}
		}
		glEnd();
		return;
	}
	
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
			//if (p[IX(i, j)]) d01 = d10 = d11 = d00 = 0.1 + (((long) p[IX(i, j)]) % 100) / 105.0; // Ferry mode
			//d01 = d10 = d11 = d00; // Henk mode
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

template <int N>
void actPolygon(Fluid &fluid, Vec verts[N], Entity *ent)
{
	const int &width = fluid.width;
	const int &height = fluid.height;
	unit fx = (unit) width / (fluid.sim->bounds.right - fluid.sim->bounds.left);
	unit fy = (unit) height / (fluid.sim->bounds.bottom - fluid.sim->bounds.top);
	
	// Sweep algorithm
	
	// Generate edges
	struct edge
	{
		unit ymin, ymax, x, s;
	};
	std::vector<edge> edges;
	for (int i = 0; i < N; ++i)
	{
		Vec u = verts[i];
		Vec v = verts[(i + 1) % N];
		u.x -= fluid.sim->bounds.left;
		v.x -= fluid.sim->bounds.left;
		u.x *= fx;	u.y *= fy;
		v.x *= fx;	v.y *= fy;
		
		unit s = (u.x - v.x) / (u.y - v.y);
		if ((1.0 / s) == 0.0) // Skip horizontal edges
			continue;
		if (u.y < v.y)
			edges.push_back({u.y, v.y, u.x, s});
		else
			edges.push_back({v.y, u.y, v.x, s});
	}
	
	// Generate event structure
	auto order = [](edge *u, edge *v) -> bool
	{
		if (u->ymin == v->ymin)
			return v->x < u->x;
		return v->ymin < u->ymin;
	};
	std::priority_queue<edge *, std::vector<edge *>, decltype(order)> events(order);
	unit ymin = height;
	unit ymax = 1.0;
	for (auto &e : edges)
	{
		if (e.ymin < ymin)
			ymin = e.ymin >= 0.0 ? e.ymin : 0.0;
		if (e.ymax > ymax)
			ymax = e.ymax <= (unit) height + 1.0 ? e.ymax : height + 1.0;
		events.push(&e);
	}
	
	// Generate status structure
	std::vector<edge *> status;
	
	// Sweep
	for (int y = round(ymin); y <= round(ymax); ++y)
	{
		while (!events.empty() && events.top()->ymin <= y)
		{
			status.push_back(events.top());
			events.pop();
		}
		status.erase(std::remove_if(status.begin(), status.end(), [y](edge *e)
		{
			return e->ymax < y;
		}), status.end());
		std::sort(status.begin(), status.end(), [](edge *u, edge *v) -> bool
		{
			return v->x > u->x;
		});
		
		int M = status.size();
		for (int i = 0; i < M; i += 2)
		{
			unit xmin = status[i]->x;
			unit xmax = i + 1 >= M ? width + 1 : status[i + 1]->x;
			if (xmin < 1.0) xmin = 1.0;
			if (xmax < 1.0) xmax = 1.0;
			if (xmin > (unit) width + 0.5) xmin = width + 0.5;
			if (xmax > (unit) width + 0.5) xmax = width + 0.5;
			
			for (int x = round(xmin); x <= round(xmax); ++x)
				fluid.p[IX(x, y)] = ent;
		}
		
		for (auto *e : status)
			e->x += e->s;
	}
}

/*void actEdge(Fluid &fluid, ParticleBase &p1, ParticleBase &p2)
{
	const int &width = fluid.width;
	const int &height = fluid.height;
	unit fx = (unit) width / (fluid.sim->bounds.right - fluid.sim->bounds.left);
	unit fy = (unit) height / (fluid.sim->bounds.bottom - fluid.sim->bounds.top);

	int i1 = (int) ((p1.x->x - fluid.sim->bounds.left) * fx);
	int j1 = (int) (p1.x->y * fy);
	int i2 = (int) ((p2.x->x - fluid.sim->bounds.left) * fx);
	int j2 = (int) (p2.x->y * fy);

	int l = (Vec(i1,j1) - Vec(i2, j2)).lengthM();
	
	unit i = i1 + 1.5;
	unit j = j1 + 1.5;
	unit di = (i2 - i1) / (unit) (l + 1);
	unit dj = (j2 - j1) / (unit) (l + 1);
	for (int k = 0; k < l + 1; ++k)
	{
		if (k < l / 2 + 1)
			fluid.p[IX((int) i, (int) j)] = &p1;
		else
			fluid.p[IX((int) i, (int) j)] = &p2;
		i += di;
		j += dj;
	}
}*/

int collidingCell(Fluid &fluid, int i, int j, unit x, unit y)
{
	const int &width = fluid.width;
	const int &height = fluid.height;
	unit fx = (unit) width / (fluid.sim->bounds.right - fluid.sim->bounds.left);
	unit fy = (unit) height / (fluid.sim->bounds.bottom - fluid.sim->bounds.top);

	int l = abs((i - x) + (j - y)) + 1;
	unit di = (i - x) / (unit) l;
	unit dj = (j - y) / (unit) l;
	int m = 0;
	for (int k = 0; k <= l + 1; ++k)
	{
		m = IX((int) x, (int) y);
		Quad *q = dynamic_cast<Quad *> (fluid.p[m]);
		RigidBase *r = dynamic_cast<RigidBase *> (fluid.p[m]);
		if (q || r)
		{
			Vec o((x / fx) + fluid.sim->bounds.left, y / fy);
			Vec v(fluid.u[m] / fx, fluid.v[m] / fy);
			if (q)
			{
				ParticleBase *p = q->p1;
				*p->f += (Vec(fluid.u[m], fluid.v[m]) - *p->v) * fluid.d[m] / fluid.speed;
				return m;
			}
			else if (r)
			{
				Vec f = (v - *r->v) * fluid.d[m] / fluid.speed;
				*r->f += f;
				*r->t += (o - *r->x) & f;
			}
		}
		x += di;
		y += dj;
	}
	return IX(i,j);
}

void couple(Fluid &fluid, unit dt)
{
	static const unit absorbtion = 0.6;
	static const unit emission = 100.0;
	const int &width = fluid.width;
	const int &height = fluid.height;
	unit fx = (unit) width / (fluid.sim->bounds.right - fluid.sim->bounds.left);
	unit fy = (unit) height / (fluid.sim->bounds.bottom - fluid.sim->bounds.top);
	
	FOR_EACH_CELL(i, j)
		Quad *q = dynamic_cast<Quad *> (fluid.p[IX(i, j)]);
		RigidBase *r = dynamic_cast<RigidBase *> (fluid.p[IX(i, j)]);
		if (q || r)
		{
			Vec v(fluid.u[IX(i, j)], fluid.v[IX(i, j)]);
			Vec c = q ? q->center() : *r->x;
			c = Vec((c.x - fluid.sim->bounds.left) * fx, c.y * fy);
			Vec n = ~(Vec(i, j) - c);
			
			// Force
			Vec px((i / fx) + fluid.sim->bounds.left, j / fy);
			Vec pv(v.x / fx, v.y / fy);
			unit d = fluid.d[IX(i, j)] / fluid.speed * emission;
			if (q)
			{
				unit q1 = (*q->p1->x - px).length2();
				unit q2 = (*q->p2->x - px).length2();
				unit q3 = (*q->p3->x - px).length2();
				unit q4 = (*q->p4->x - px).length2();
				unit qs = q1 + q2 + q3 + q4;
				q1 = 1.0 - (q1 / qs);
				q2 = 1.0 - (q2 / qs);
				q3 = 1.0 - (q3 / qs);
				q4 = 1.0 - (q4 / qs);
				*q->p1->f -= (pv - *q->p1->v) * d * q1 / 25.0;
				*q->p2->f -= (pv - *q->p2->v) * d * q2 / 25.0;
				*q->p3->f -= (pv - *q->p3->v) * d * q3 / 25.0;
				*q->p4->f -= (pv - *q->p4->v) * d * q4 / 25.0;
			}
			else if (r)
			{
				Vec f = (pv - *r->v) * d;
				*r->f += f;
				*r->t += (px - *r->x) & f;
			}
			
			// Reflection
			v *= absorbtion;
			if (v * n <= 0.0) // only reflect opposing forces
			{
				v = v - 2.0 * (v * n) * n;
				fluid.u[IX(i, j)] = v.x;
				fluid.v[IX(i, j)] = v.y;
			}
			
			// De-advection
			v = ~v;
			int i0 = CLAMPX(i + v.x + 0.5);
			int j0 = CLAMPY(j + v.y + 0.5);
			int i1 = CLAMPX(i + v.x + n.x + 0.5);
			int j1 = CLAMPY(j + v.y + n.y + 0.5);
			fluid.d[IX(i0, j0)] += fluid.d[IX(i, j)] * 0.25;
			fluid.d[IX(i0, j1)] += fluid.d[IX(i, j)] * 0.25;
			fluid.d[IX(i1, j0)] += fluid.d[IX(i, j)] * 0.25;
			fluid.d[IX(i1, j1)] += fluid.d[IX(i, j)] * 0.25;
			unit miss = 0.0;
			if (i == i0 && j == j0) miss += 0.25;
			if (i == i0 && j == j1) miss += 0.25;
			if (i == i1 && j == j0) miss += 0.25;
			if (i == i1 && j == j1) miss += 0.25;
			fluid.d[IX(i, j)] *= miss;
		}
	END_FOR
}

void Fluid::act(unit dt)
{
	const size_t size = (width + 2)*(height + 2);
	
	dt *= speed;
	
	// Reset, apply gravity
	for (int i = 0; i < size; ++i)
	{
		u_old[i] = g.x * d[i];
		v_old[i] = g.y * d[i];
		d_old[i] = 0.0;
		p[i] = NULL;
	}

	// Collisions
	for (Quad **q = sim->getQuads(); *q; ++q)
	{
		Vec ps[4] = {
			*(*q)->p1->x,
			*(*q)->p2->x,
			*(*q)->p3->x,
			*(*q)->p4->x};
		actPolygon<4>(*this, ps, *q);
	}
	for (RigidBase **r = sim->getRigids(); *r; ++r)
	{
		RigidBox *rb = dynamic_cast<RigidBox *> (*r);
		if (!rb) continue;
		const unit cs = rb->size / 2.0;
		Vec n = Vec::fromAngle(*(*r)->o);
		Vec ps[4] = {
			(Vec(-cs, -cs) ^ n) + *(*r)->x,
			(Vec(-cs,  cs) ^ n) + *(*r)->x,
			(Vec( cs,  cs) ^ n) + *(*r)->x,
			(Vec( cs, -cs) ^ n) + *(*r)->x};
		actPolygon<4>(*this, ps, *r);
	}
	
	// Mouse interaction
	{
		int i = (int) (width * mouse.pos.x + 0.5);
		int j = (int) (height * mouse.pos.y + 0.5);
		
		if (i < 0) i = 0;
		if (i >= width) i = width -1;
		if (j < 0) j = 0;
		if (j >= height) j = height -1;
		
		u_old[IX(i,j)] += mouse.v.x;
		v_old[IX(i,j)] += mouse.v.y;
		d_old[IX(i,j)] += mouse.d;
		mouse.v = Vec();
		mouse.d = 0;
	}

	vel_step(u, v, u_old, v_old, visc, dt);
	dens_step(d, d_old, u, v, diff, dt);
	couple(*this, dt);
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
	unit x, y, s0, t0, s1, t1, ds0, dt0;
	ds0 = dt * width;
	dt0 = dt * height;
	FOR_EACH_CELL(i, j)
		x = i - ds0 * u[IX(i, j)];
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
		d[IX(i, j)] =
		//d[collidingCell(*this, i, j, x, y)] +=
			s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
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
	const size_t size = (width + 2)*(height + 2);
	for (int i = 0; i < size; ++i)
		x[i] = 0.0;
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
