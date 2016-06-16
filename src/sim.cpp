/*************************************************************
 * Simulation class -- See header file for more information. *
 *************************************************************/

#include <ostream>
#include <vector>

#include "sim.h"
#include "integrators.h"

namespace Sim {

//------------------------------------------------------------------------------

struct Simulation::Data
{
	std::vector<Entity *> entities;
	std::vector<ParticleBase *>particles;
	std::vector<Quad *>quads;
	ParticleSystem system;
	ParticleSystem cache;
};

//------------------------------------------------------------------------------

Simulation::Simulation(const char *title)
	: Window(title), data(new Data)
{
	data->particles.push_back(NULL);
	data->quads.push_back(NULL);
}

Simulation::~Simulation()
{
	clear();
	delete data;
}

void Simulation::manage(Entity *ent)
{
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

//------------------------------------------------------------------------------

ParticleSystem &Simulation::getSystem()
{
	return data->system;
}

void Simulation::calcForces()
{
	// Reset forces
	std::fill(data->system.f.begin(), data->system.f.end(), Vec());
	// Apply forces
	for (Entity *ent : data->entities)
		if (dynamic_cast<Appliable *> (ent))
			dynamic_cast<Appliable *> (ent)->apply();
}

void Simulation::saveState()
{
	data->cache.x = data->system.x;
	data->cache.v = data->system.v;
}

void Simulation::restoreState()
{
	data->system.x = data->cache.x;
	data->system.v = data->cache.v;
	update_pointers();
}

//------------------------------------------------------------------------------

void Simulation::clear()
{
	for (Entity *ent : data->entities)
		delete ent;
	data->entities.clear();
	data->particles.clear();
	data->particles.push_back(NULL);
	data->quads.clear();
	data->quads.push_back(NULL);
	data->system = ParticleSystem();
	data->cache = ParticleSystem();
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

Quad **Simulation::getQuads()
{
	return data->quads.data();
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
