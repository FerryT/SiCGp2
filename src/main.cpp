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
#include "effects.h"

using namespace Sim;

//------------------------------------------------------------------------------

class Main : public Simulation
{
public:
	Integrator *integrator = NULL;
	unit dt = 0.001;
	
	Fluid *fluid = NULL;
	Texture *t1 = NULL;
	Entity *selected = NULL;
	
	template <int I> void gotoScene();
	
	Main(const char *title);
	void reset();
	void preact();
	void postact();
	
	void keypress(unsigned char key);
	void mouseup(const GUI::MouseEvent &);
	void mousedown(const GUI::MouseEvent &);
	void mousemove(const GUI::MouseEvent &);
	
	inline Vec getMouse();
	inline Vec normalPosition(int x, int y);
	
	static Main *instance;
	static void Frame()
	{
		instance->preact();
		if (instance->integrator)
			instance->act(*(instance->integrator), instance->dt);
		instance->postact();
	}

private:
	int scene = 1;
	struct { int x, y, dx, dy; int down; } mouse;
};

Main *Main::instance = NULL;

void createCloth(Simulation *sim, unit x, unit y, unit w, unit h, int rx, int ry,
	unit ks, unit kd, Texture *tex = NULL);

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
		
		Texture texture("cloth.raw", 477, 477);
		sim.t1 = &texture;
		
		GUI::Run(Main::Frame);
	}
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------

template <> void Main::gotoScene<1>()
{
	Vec G(0, -10.0);
	fluid = create<Fluid>(this, 30, 30, 0.0001, 0.000001, G);
}

template <> void Main::gotoScene<2>()
{
	Vec G(0, -1.0);
	fluid = create<Fluid>(this, 30, 30, 0.0001, 0.000001, G);
	createCloth(this, 0.25, 0.25, .5, .5, 10, 10, -1000, -100, t1);
	create<Gravity>(this, G, 0.0);
}

template <> void Main::gotoScene<3>()
{

}

template <> void Main::gotoScene<4>()
{
}

template <> void Main::gotoScene<5>()
{
}

//------------------------------------------------------------------------------

void createCloth(Simulation *sim, unit x, unit y, unit w, unit h, int rx, int ry,
	unit ks, unit kd, Texture *tex)
{
	std::map< int, std::map<int,ParticleBase *> > grid;
	
	unit dx = w / (unit) rx;
	unit dy = h / (unit) ry;
	
	for (int i = 0; i < rx; ++i)
		for (int j = 0; j < ry; ++j)
			grid[i][j] = sim->addParticle(Vec(x + (i * dx), y + (j * dy)));
	
	for (int i = 0; i < rx-1; ++i)
		for (int j = 0; j < ry; ++j)
			sim->create<Spring>(grid[i][j], grid[i+1][j], dx, ks, kd);
	
	for (int j = 0; j < ry-1; ++j)
		for (int i = 0; i < rx; ++i)
			sim->create<Spring>(grid[i][j], grid[i][j+1], dy, ks, kd);
	
	dx = 1.0 / (unit) (rx-1);
	dy = 1.0 / (unit) (ry-1);
	
	if (!tex)
		for (int i = 0; i < rx-1; ++i)
			for (int j = 0; j < ry-1; ++j)
				sim->create<Quad>(grid[i][j], grid[i+1][j], grid[i+1][j+1], grid[i][j+1]);
	else
		for (int i = 0; i < rx-1; ++i)
			for (int j = 0; j < ry-1; ++j)
				sim->create<Sheet>(grid[i][j], grid[i+1][j], grid[i+1][j+1], grid[i][j+1],
				Vec((unit) i * dx, (unit) j * dy),
				Vec((unit) (i+1) * dx, (unit) j * dy),
				Vec((unit) (i+1) * dx, (unit) (j+1) * dy),
				Vec((unit) i * dx, (unit) (j+1) * dy), tex);

	sim->create<Glue>(grid[0][ry-1], *grid[0][ry-1]->x);
	sim->create<Glue>(grid[rx-1][ry-1], *grid[rx-1][ry-1]->x);
}

//------------------------------------------------------------------------------

Main::Main(const char *title) : Simulation(title), mouse({0,0,0,0,0})
{
	Main::instance = this;
	reset();
}

//------------------------------------------------------------------------------

template <> void Main::gotoScene<0>()
{
	switch (scene)
	{
		case 1: gotoScene<1>(); break;
		case 2: gotoScene<2>(); break;
		case 3: gotoScene<3>(); break;
		case 4: gotoScene<4>(); break;
		case 5: gotoScene<5>(); break;
		default: gotoScene<1>(); break;
	}
}

//------------------------------------------------------------------------------

void Main::reset()
{
	clear();
	gotoScene<0>();
}

//------------------------------------------------------------------------------

void Main::preact()
{
	//if (scene == 1)
	if (fluid)
	{
		fluid->mouse.pos = normalPosition(mouse.x, mouse.y);
		if (mouse.down & GUI::MouseEvent::btnLeft)
			fluid->mouse.v = normalPosition(mouse.dx, mouse.dy) * 300.0;
		if (mouse.down & GUI::MouseEvent::btnRight)
		{
			fluid->mouse.v = Vec(300.0,0.0);
			if (mouse.down & GUI::MouseEvent::btnLeft)
				fluid->mouse.d = -1000.0;
			else
				fluid->mouse.d = 1000.0;
		}
	}
}

//------------------------------------------------------------------------------

void Main::postact()
{
	/*if (selected)
	{
		ParticleBase *pb = dynamic_cast<ParticleBase *> (selected);
		if (pb)
		{
			*pb->x = getMouse();
			*pb->v = Vec();
			*pb->f = Vec();
		}
	}*/
}

//------------------------------------------------------------------------------

void Main::keypress(unsigned char key)
{
	if (key >= '0' && key <= '9')
	{
		scene = key - '0';
		reset();
		return;
	}
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
	mouse.dx = event.x - mouse.x;
	mouse.dy = event.y - mouse.y;
	mouse.x = event.x;
	mouse.y = event.y;
}

void Main::mouseup(const GUI::MouseEvent &event)
{
	mouse.down = mouse.down & ~event.button;
	selected = NULL;
}

void Main::mousedown(const GUI::MouseEvent &event)
{
	mouse.x = event.x;
	mouse.y = event.y;
	mouse.down |= event.button;
	
	if (scene == 2)
	{
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
}

//------------------------------------------------------------------------------

inline Vec Main::getMouse()
{
	unit x = ((bounds.right - bounds.left) / (unit) width) * (unit) mouse.x;
	unit y = ((bounds.bottom - bounds.top) / (unit) height) * (unit) mouse.y;
	return Vec(bounds.left + x, bounds.bottom - y);
}

inline Vec Main::normalPosition(int x, int y)
{
	return Vec((unit) x / (unit) width,
		1.0 - ((unit) y / (unit) height));
}

//------------------------------------------------------------------------------
