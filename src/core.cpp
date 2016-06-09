/*************************************************************
 * Entity structure -- See header file for more information. *
 *************************************************************/

#include "GL/freeglut.h"

#include "core.h"

namespace Sim {

//------------------------------------------------------------------------------

void Particle::draw()
{
	static const double h = 0.015;
	static const double white[3] = {1.0, 1.0, 1.0};
	
	glColor3dv(white);
	
	glBegin(GL_QUADS);
	glVertex2d(x.x - h, x.y - h);
	glVertex2d(x.x + h, x.y - h);
	glVertex2d(x.x + h, x.y + h);
	glVertex2d(x.x - h, x.y + h);
	glEnd();
}

//------------------------------------------------------------------------------

std::ostream &operator <<(std::ostream &out, const Particle &p)
{
	return out << "P< " << p.x << ", " << p.v
		<< ", " << p.f << ", " << p.m << " >";
}

//------------------------------------------------------------------------------

ParticleSystem::ParticleSystem(size_t _count)
	: count(_count), particles(new Particle[_count])
{
}

ParticleSystem::ParticleSystem(const ParticleSystem &other)
	: count(other.count), particles(new Particle[other.count])
{
	std::copy(other.particles, other.particles + other.count, particles);
}

ParticleSystem::ParticleSystem(ParticleSystem &&other)
	: count(other.count), particles(other.particles)
{
	other.particles = NULL;
}

ParticleSystem::~ParticleSystem()
{
	if (particles)
		delete[] particles;
}

//------------------------------------------------------------------------------

void ParticleSystem::copy(const ParticleSystem &other)
{
	size_t l = count < other.count ? count : other.count;
	std::copy(other.particles, other.particles + l, particles);
}

void ParticleSystem::copyForces(const ParticleSystem &other)
{
	size_t l = count < other.count ? count : other.count;
	for (size_t i = 0; i < l; ++i)
		particles[i].f = other.particles[i].f;
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//------------------------------------------------------------------------------
