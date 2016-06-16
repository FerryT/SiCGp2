/*******************************************************
 * Effects -- header file                              *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *                                                     *
 * Date: 22:31 16-6-2016                               *
 *                                                     *
 * Description: Visual adorments                       *
 *******************************************************/

#ifndef _EFFECTS_H
#define _EFFECTS_H

#include "core.h"

namespace Sim {

//------------------------------------------------------------------------------

class Texture
{
public:
	const unsigned int &index;
	
	Texture(const char *file, int width, int height);
	~Texture();
	
private:
	unsigned char *buffer;
	unsigned int index_v;
};

//------------------------------------------------------------------------------

class Sheet : public Quad, virtual public Drawable
{
public:
	Vec c1, c2, c3, c4;
	Texture *tex;
	
	Sheet(ParticleBase *p1, ParticleBase *p2, ParticleBase *p3, ParticleBase *p4,
	Vec _c1, Vec _c2, Vec _c3, Vec _c4, Texture *_tex)
		: Quad(p1, p2, p3, p4), c1(_c1), c2(_c2), c3(_c3), c4(_c4), tex(_tex) {}
	
	void draw();
};

//------------------------------------------------------------------------------

} /* namespace Sim */

#endif /* _EFFECTS_H */

//..............................................................................
