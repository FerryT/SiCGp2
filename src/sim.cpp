/*************************************************************
 * Simulation class -- See header file for more information. *
 *************************************************************/

#include <vector>

#include "sim.h"

namespace Sim {

//------------------------------------------------------------------------------

struct Simulation::Data
{
	std::vector<Entity *> entities;
	ParticleSystem *system;
};

//------------------------------------------------------------------------------

Simulation::Simulation(const char *title) : Window(title), data(new Data)
{
	data->system = NULL;
}

Simulation::~Simulation()
{
	clear();
	delete data;
}

void Simulation::manage(Entity *ent)
{
	data->entities.push_back(ent);
	
	// Generate new system, every addition, not efficient but care
	if (dynamic_cast<Particle *> (ent))
	{
		ParticleSystem *ps;
		if (!data->system)
			ps = new ParticleSystem(1);
		else
		{
			ps = new ParticleSystem(data->system->count + 1);
			ps->copy(*data->system);
			delete data->system;
		}
		ps->particles[ps->count - 1] = *(dynamic_cast<Particle *> (ent));
		data->system = ps;
	}
}

void Simulation::clear()
{
	for (Entity *ent : data->entities)
		delete ent;
	data->entities.clear();
	delete data->system;
	data->system = NULL;
}

//------------------------------------------------------------------------------

void Simulation::draw()
{
	for (Entity *ent : data->entities)
		if (dynamic_cast<Drawable *> (ent))
			dynamic_cast<Drawable *> (ent)->draw();
}

//------------------------------------------------------------------------------

void Simulation::act()
{
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
