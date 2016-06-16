/****************************************************
 * Effects -- See header file for more information. *
 ****************************************************/

#include <stdio.h>

#include "GL/freeglut.h"

#include "effects.h"

namespace Sim {

//------------------------------------------------------------------------------

Texture::Texture(const char *file, int width, int height) : index(index_v)
{
	FILE *fp = fopen(file, "rb");
	size_t size = width * height * 3;
	buffer = new unsigned char[size];
	fread(buffer, size, 1, fp);
	fclose(fp);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, &index_v);
	glBindTexture(GL_TEXTURE_2D, index_v);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, buffer);
}

//------------------------------------------------------------------------------

Texture::~Texture()
{
	delete[] buffer;
}

//------------------------------------------------------------------------------

void Sheet::draw()
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, (GLuint) tex->index);
	glBegin(GL_QUADS);
	glTexCoord2dv(c1.data);
	glVertex2dv(p1->x->data);
	glTexCoord2dv(c2.data);
	glVertex2dv(p2->x->data);
	glTexCoord2dv(c3.data);
	glVertex2dv(p3->x->data);
	glTexCoord2dv(c4.data);
	glVertex2dv(p4->x->data);
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

} /* namespace Sim */

//..............................................................................
