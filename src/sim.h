/*******************************************************
 * Simulation class -- header file                     *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *                                                     *
 * Date: 22:16 6-6-2016                                *
 *                                                     *
 * Description: Main component that controls the       *
 *              simulation                             *
 *******************************************************/

#ifndef _SIM_H
#define _SIM_H

#include "gui.h"
#include "core.h"
#include "forces.h"

namespace Sim {

//------------------------------------------------------------------------------

class Simulation : public GUI::Window
{
public:
	Simulation(const char *title);
	~Simulation();
	
	template <class T, typename... A> inline T *create(A... args)
		{ T *ptr = new T(args...); manage(ptr); return ptr; }
	void clear();
	
	void act();

private:
	void draw();
	void manage(Entity *);
	struct Data;
	Data *data;
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _SIM_H */

//..............................................................................
