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

#include "simulation.h"

using namespace Sim;

//------------------------------------------------------------------------------

Simulation *sim;

int main(int argc, char *argv[])
{
	GUI::Init(&argc, argv);
	{
		Simulation sim1("Fluid yet rigid");
		
		sim = &sim1;
		GUI::Run([]{ sim->act(); });
	}
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
