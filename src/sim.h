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
	const ParticleSystem &system;
	
	Simulation(const char *title);
	~Simulation();
	
	template <class T, typename... A> inline T *create(A... args)
		{ T *ptr = new T(args...); manage(ptr); return ptr; }
	template <typename... A> inline ParticleBase *create(A... args)
		{ Particle p(args...); return manage(p); }
	void clear();
	
	void act();

protected:
	void calcForces();
	void setForces(const Vecs &);
	void saveState();
	void restoreState();

private:
	void draw();
	void manage(Entity *);
	ParticleBase *manage(const Particle &);
	void update_pointers();
	struct Data;
	Data *data;
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _SIM_H */

//..............................................................................
