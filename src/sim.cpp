/*************************************************************
 * Simulation class -- See header file for more information. *
 *************************************************************/

#include <ostream>
#include <vector>

#include "fluid.h"
#include "sim.h"
#include "integrators.h"

namespace Sim {

//------------------------------------------------------------------------------

struct Simulation::Data
{
	std::vector<Entity *> entities;
	std::vector<ParticleBase *>particles;
	std::vector<RigidBase *>rigids;
	std::vector<Quad *>quads;
	ParticleSystem system;
	RigidSystem system2;
	ParticleSystem cache;
	RigidSystem cache2;
};

//------------------------------------------------------------------------------

Simulation::Simulation(const char *title)
	: Window(title), data(new Data)
{
	data->particles.push_back(NULL);
	data->rigids.push_back(NULL);
	data->quads.push_back(NULL);
}

Simulation::~Simulation()
{
	clear();
	delete data;
}

void Simulation::manage(Entity *ent)
{
	if (dynamic_cast<Fluid *> (ent) && !data->entities.empty())
	{
		data->entities.push_back(data->entities[0]);
		data->entities[0] = ent;
		return;	
	}
	data->entities.push_back(ent);
	Quad *q = dynamic_cast<Quad *> (ent);
	if (q)
	{
		data->quads.back() = q;
		data->quads.push_back(NULL);
	}
}

ParticleBase *Simulation::manage(const Particle &p)
{
	data->system.x.push_back(p.x);
	data->system.v.push_back(p.v);
	data->system.f.push_back(p.f);
	data->system.m.push_back(p.m);
	++data->system.size;
	ParticleBase *pb = new ParticleBase();
	data->entities.push_back(pb);
	data->particles.back() = pb;
	data->particles.push_back(NULL);
	update_pointers();
	return pb;
}

RigidBase *Simulation::manage(RigidBody *r)
{
	data->system2.x.push_back(r->x);
	data->system2.v.push_back(r->v);
	data->system2.f.push_back(r->f);
	data->system2.o.push_back(r->o);
	data->system2.w.push_back(r->w);
	data->system2.t.push_back(r->t);
	data->system2.m.push_back(r->m);
	data->system2.i.push_back(r->body());
	++data->system2.size;
	data->entities.push_back(r);
	data->rigids.back() = r;
	data->rigids.push_back(NULL);
	update_pointers2();
	return r;
}

void Simulation::update_pointers()
{
	auto x = data->system.x.data();
	auto v = data->system.v.data();
	auto f = data->system.f.data();
	auto m = data->system.m.data();
	for (size_t i = 0; i < data->particles.size() - 1; ++i)
	{
		data->particles[i]->x = &x[i];
		data->particles[i]->v = &v[i];
		data->particles[i]->f = &f[i];
		data->particles[i]->m = &m[i];
	}
}

void Simulation::update_pointers2()
{
	auto x = data->system2.x.data();
	auto v = data->system2.v.data();
	auto f = data->system2.f.data();
	auto o = data->system2.o.data();
	auto w = data->system2.w.data();
	auto t = data->system2.t.data();
	auto m = data->system2.m.data();
	for (size_t i = 0; i < data->rigids.size() - 1; ++i)
	{
		data->rigids[i]->x = &x[i];
		data->rigids[i]->v = &v[i];
		data->rigids[i]->f = &f[i];
		data->rigids[i]->o = &o[i];
		data->rigids[i]->w = &w[i];
		data->rigids[i]->t = &t[i];
		data->rigids[i]->m = &m[i];
	}
}

//------------------------------------------------------------------------------

ParticleSystem &Simulation::getSystem()
{
	return data->system;
}

RigidSystem &Simulation::getSystem2()
{
	return data->system2;
}

void Simulation::calcForces()
{
	// Reset forces
	std::fill(data->system.f.begin(), data->system.f.end(), Vec());
	std::fill(data->system2.f.begin(), data->system2.f.end(), Vec());
	std::fill(data->system2.t.begin(), data->system2.t.end(), Vec());
	// Apply forces
	for (Entity *ent : data->entities)
		if (dynamic_cast<Appliable *> (ent))
			dynamic_cast<Appliable *> (ent)->apply();
}

void Simulation::saveState()
{
	data->cache.x = data->system.x;
	data->cache.v = data->system.v;
	data->cache2.x = data->system2.x;
	data->cache2.v = data->system2.v;
	data->cache2.o = data->system2.o;
	data->cache2.w = data->system2.w;
}

void Simulation::restoreState()
{
	data->system.x = data->cache.x;
	data->system.v = data->cache.v;
	update_pointers();
	data->system2.x = data->cache2.x;
	data->system2.v = data->cache2.v;
	data->system2.o = data->cache2.o;
	data->system2.w = data->cache2.w;
	update_pointers2();
}

//------------------------------------------------------------------------------

void Simulation::clear()
{
	for (Entity *ent : data->entities)
		delete ent;
	data->entities.clear();
	data->particles.clear();
	data->particles.push_back(NULL);
	data->rigids.clear();
	data->rigids.push_back(NULL);
	data->quads.clear();
	data->quads.push_back(NULL);
	data->system = ParticleSystem();
	data->cache = ParticleSystem();
	data->system2 = RigidSystem();
	data->cache2 = RigidSystem();
}

//------------------------------------------------------------------------------

void Simulation::draw()
{
	for (Entity *ent : data->entities)
		if (dynamic_cast<Drawable *> (ent))
			dynamic_cast<Drawable *> (ent)->draw();
}

//------------------------------------------------------------------------------

void Simulation::act(Integrator &intg, unit h)
{
	calcForces();
	for (Entity *ent : data->entities)
		if (dynamic_cast<Actor *> (ent))
			dynamic_cast<Actor *> (ent)->act(h);
	intg.integrate(h);
}

//------------------------------------------------------------------------------

ParticleBase **Simulation::getParticles()
{
	return data->particles.data();
}

//------------------------------------------------------------------------------

RigidBase **Simulation::getRigids()
{
	return data->rigids.data();
}

//------------------------------------------------------------------------------

Quad **Simulation::getQuads()
{
	return data->quads.data();
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
