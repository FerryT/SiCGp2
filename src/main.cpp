/*******************************************************
 * Main file -- header file                            *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *                                                     *
 * Date: 15:50 3-6-2016                                *
 *                                                     *
 * Description: Fluid and rigid body simulation        *
 *******************************************************/

#include <stdlib.h>
#include <map>

#include "GL/freeglut.h"

#include "sim.h"
#include "integrators.h"
#include "fluid.h"

using namespace Sim;

//------------------------------------------------------------------------------

class Main : public Simulation
{
public:
	Integrator *integrator = NULL;
	unit dt = 0.001;
	
	Fluid *fluid;
	Entity *selected = NULL;
	
	Main(const char *title);
	
	void reset();
	void preact();
	void postact();
	
	void keypress(unsigned char key);
	void mouseup(const GUI::MouseEvent &);
	void mousedown(const GUI::MouseEvent &);
	void mousemove(const GUI::MouseEvent &);
	
	inline Vec getMouse();
	
	static Main *instance;
	static void Frame()
	{
		instance->preact();
		if (instance->integrator)
			instance->act(*(instance->integrator), instance->dt);
		instance->postact();
	}

private:
	struct { int x, y; bool down; } mouse;
};

Main *Main::instance = NULL;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	GUI::Init(&argc, argv);
	{
		Main sim("Fluid yet rigid");
		Verlet verlet(sim);
		Euler euler(sim);
		MidPoint<Verlet> midpoint(sim);
		RungeKutta4< RungeKutta4<Verlet> > superrunge(sim);
		sim.integrator = &verlet;
		
		GUI::Run(Main::Frame);
	}
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

void createCloth(Simulation *sim, unit x, unit y, unit w, unit h, int rx, int ry,
	unit ks, unit kd)
{
	std::map< int, std::map<int,ParticleBase *> > grid;
	
	const unit dx = w / (unit) rx;
	const unit dy = h / (unit) ry;
	
	for (int i = 0; i < rx; ++i)
		for (int j = 0; j < ry; ++j)
			grid[i][j] = sim->addParticle(Vec(x + (i * dx), y + (j * dy)));
	
	for (int i = 0; i < rx-1; ++i)
		for (int j = 0; j < ry; ++j)
			sim->create<Spring>(grid[i][j], grid[i+1][j], dx, ks, kd);
	
	for (int j = 0; j < ry-1; ++j)
		for (int i = 0; i < rx; ++i)
			sim->create<Spring>(grid[i][j], grid[i][j+1], dy, ks, kd);
	
	sim->create<Glue>(grid[0][ry-1], *grid[0][ry-1]->x);
	sim->create<Glue>(grid[rx-1][ry-1], *grid[rx-1][ry-1]->x);
}

//------------------------------------------------------------------------------

Main::Main(const char *title) : Simulation(title), mouse({0,0, false})
{
	Main::instance = this;
	reset();
}

//------------------------------------------------------------------------------

void Main::reset()
{
	clear();
	
	Vec G(0, -0.05);
	fluid = create<Fluid>(this, 20, 20, 0.0001, 0.000001, G * 50.0);
	createCloth(this, 0.25, 0.5, .5, .5, 30, 30, -1000, -100);
	create<Gravity>(this, G, 0.0);
}

//------------------------------------------------------------------------------

void Main::preact()
{
}

//------------------------------------------------------------------------------

void Main::postact()
{
	if (selected)
	{
		ParticleBase *pb = dynamic_cast<ParticleBase *> (selected);
		if (pb)
		{
			*pb->x = getMouse();
			*pb->v = Vec();
			*pb->f = Vec();
		}
	}
}

//------------------------------------------------------------------------------

void Main::keypress(unsigned char key)
{
	switch (key)
	{
		case 'q':
		case '\e':
			exit(0);
			break;
		
		case GLUT_KEY_F5:
		case 'r':
			reset();
			break;
	}
}

//------------------------------------------------------------------------------

void Main::mousemove(const GUI::MouseEvent &event)
{
	mouse.x = event.x;
	mouse.y = event.y;
}

void Main::mouseup(const GUI::MouseEvent &)
{
	mouse.down = false;
	selected = NULL;
}

void Main::mousedown(const GUI::MouseEvent &event)
{
	mouse.x = event.x;
	mouse.y = event.y;
	mouse.down = true;
	
	Vec m = getMouse();
	unit min = 1.0 / 0.0;
	selected = NULL;
	for (ParticleBase **pb = getParticles(); *pb; ++pb)
	{
		unit l = (*(**pb).x - m).length();
		if (l < min)
		{
			min = l;
			selected = *pb;
		}
	}
}

inline Vec Main::getMouse()
{
	unit x = ((bounds.right - bounds.left) / (unit) width) * (unit) mouse.x;
	unit y = ((bounds.bottom - bounds.top) / (unit) height) * (unit) mouse.y;
	return Vec(bounds.left + x, bounds.bottom - y);
}

//------------------------------------------------------------------------------
