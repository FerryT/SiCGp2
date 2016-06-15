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
	unit dt = 0.01;
	
	Fluid *fluid;
	
	Main(const char *title);
	void reset();
	
	void keypress(unsigned char key);
	void mouseup(const GUI::MouseEvent &);
	
	static Main *instance;
	static void Frame()
	{
		if (instance->integrator)
			instance->act(*(instance->integrator), instance->dt);
	}
};

Main *Main::instance = NULL;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	GUI::Init(&argc, argv);
	{
		Main sim("Fluid yet rigid");
		Verlet verlet(sim);
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
}

//------------------------------------------------------------------------------

Main::Main(const char *title) : Simulation(title)
{
	Main::instance = this;
	reset();
}

//------------------------------------------------------------------------------

void Main::reset()
{
	clear();
	
	Vec G(0, -0.05);
	
	fluid = create<Fluid>(this, 100, 100, 0.05, 0.000001, G * 50.0);
	createCloth(this, 0.25, 0.25, .5, .5, 4, 4, 1, 1);
	create<Gravity>(this, G, 0.0);
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

void Main::mouseup(const GUI::MouseEvent &event)
{
}

//------------------------------------------------------------------------------
