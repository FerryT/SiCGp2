/*************************************************************
 * Simulation class -- See header file for more information. *
 *************************************************************/

#include <ostream>
#include <vector>

#include "sim.h"

namespace Sim {

//------------------------------------------------------------------------------

struct Simulation::Data
{
	std::vector<Entity *> entities;
	ParticleSystem system;
	ParticleSystem cache;
};

//------------------------------------------------------------------------------

Simulation::Simulation(const char *title)
	: Window(title), data(new Data), system(data->system)
{
}

Simulation::~Simulation()
{
	clear();
	delete data;
}

void Simulation::manage(Entity *ent)
{
	data->entities.push_back(ent);
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
	update_pointers();
	return pb;
}

void Simulation::update_pointers()
{
	auto x = data->system.x.begin();
	auto v = data->system.v.begin();
	auto f = data->system.f.begin();
	auto m = data->system.m.begin();
	for (Entity *ent : data->entities)
	{
		ParticleBase *pb = dynamic_cast<ParticleBase *> (ent);
		if (pb)
		{
			pb->x = &*x++;
			pb->v = &*v++;
			pb->f = &*f++;
			pb->m = &*m++;
		}
	}
}

//------------------------------------------------------------------------------

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
}

//------------------------------------------------------------------------------

void Simulation::clear()
{
	for (Entity *ent : data->entities)
		delete ent;
	data->entities.clear();
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
	intg.integrate(h);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
