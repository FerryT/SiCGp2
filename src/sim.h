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
#include "base.h"
#include "core.h"
#include "forces.h"
#include "rigid.h"

namespace Sim {

class Integrator;

//------------------------------------------------------------------------------

class Simulation : public GUI::Window
{
public:
	Simulation(const char *title);
	~Simulation();
	
	template <class T, typename... A> inline T *create(A... args)
		{ T *ptr = new T(args...); manage(ptr); return ptr; }
	template <typename... A>inline ParticleBase *addParticle(A... args)
		{ return manage(Particle(args...)); }
	template <class T, typename... A>inline RigidBase *addRigid(A... args)
		{ return manage(new T(args...)); }
	void clear();
	
	friend class Integrator;
	virtual void act(Integrator &, unit h);
	
	ParticleBase **getParticles();
	RigidBase **getRigids();
	Quad **getQuads();

protected:
	ParticleSystem &getSystem();
	RigidSystem &getSystem2();
	void calcForces();
	void saveState();
	void restoreState();

private:
	void draw();
	void manage(Entity *);
	ParticleBase *manage(const Particle &);
	RigidBase *manage(RigidBody *);
	void update_pointers();
	void update_pointers2();
	struct Data;
	Data *data;
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _SIM_H */

//..............................................................................
