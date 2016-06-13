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

#include "sim.h"

using namespace Sim;

//------------------------------------------------------------------------------

Simulation *sim;
Integrator *intg;

int main(int argc, char *argv[])
{
	GUI::Init(&argc, argv);
	{
		Simulation sim1("Fluid yet rigid");
		Euler int1(sim1);
		
		sim = &sim1;
		intg = &int1;
		GUI::Run([]{ sim->act(*intg, 0.01); });
	}
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
