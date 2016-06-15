/*********************************************************************
 * Graphical user interface -- See header file for more information. *
 *********************************************************************/

#include <stdlib.h>
#include <iostream>
#include <map>

#include "GL/freeglut.h"

#include "gui.h"

namespace GUI {

const int default_width = 800;
const int default_height = 600;

std::map<int,Window *> windows;

//------------------------------------------------------------------------------

inline MouseEvent::Button to_button(int button)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON: return MouseEvent::Button::btnLeft;
		case GLUT_MIDDLE_BUTTON: return MouseEvent::Button::btnMiddle;
		case GLUT_RIGHT_BUTTON: return MouseEvent::Button::btnRight;
	}
}

inline Window *current_window()
{
	auto it = windows.find(glutGetWindow());
	if (it == windows.end())
		return NULL;
	return it->second;
}

//------------------------------------------------------------------------------

void Window::reshape_func(int width, int height)
{
	Window *win = current_window();
	if (!win) return;
	
	win->cache.width = width;
	win->cache.height = height;
	win->cache.ratio = (float) width / (float) height;
	win->bounds_v.left = (1.0 - win->cache.ratio) / 2.0;
	win->bounds_v.right = 1.0 - win->bounds_v.left;
	win->resized();
}

void Window::keyboard_func(unsigned char key, int x, int y)
{
	Window *win = current_window();
	if (!win) return;
	
	if (key > 'Z')
		key -= 'a' - 'A';
	win->keypress(key);
}

void Window::special_func(int key, int x, int y)
{
	Window *win = current_window();
	if (!win) return;
	
	win->keypress(key);
}

void Window::mouse_func(int button, int state, int x, int y)
{
	Window *win = current_window();
	if (!win) return;
	auto last = win->buttons[button];
	
	if (state == GLUT_DOWN)
	{
		MouseEvent e = {x, y, to_button(button)};
		win->mousedown(e);
	}
	else if (state == GLUT_UP)
	{
		MouseEvent e = {x, y, to_button(button)};
		win->mouseup(e);
		if (last.x == x && last.y == y)
			win->mousepress(e);
	}
	last.x = x;
	last.y = y;
	last.state = state;
}

void Window::motion_func(int x, int y)
{
	Window *win = current_window();
	if (!win) return;
	
	int buttons = 0;
	for (int i = buttoncount - 1; i >= 0; --i)
		if (win->buttons[i].state == GLUT_DOWN)
			buttons |= to_button(i);
	
	MouseEvent e = {x, y, to_button(buttons)};
	win->mousemove(e);
}

void Window::display_func()
{
	Window *win = current_window();
	if (!win) return;
	
	win->predraw();
	win->draw();
	win->postdraw();
}

//------------------------------------------------------------------------------

Window::Window(const char *title)
	: bounds(bounds_v), x(*this), y(*this), width(*this), height(*this),
	bounds_v({0.0, 1.0, 0.0, 1.0}),
	cache({0, 0, default_width, default_height,
		(float) default_width / (float) default_height})
{
	int default_x = (glutGet(GLUT_SCREEN_WIDTH) - default_width) / 2;
	int default_y = (glutGet(GLUT_SCREEN_HEIGHT) - default_height) / 2;
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(default_x, default_y);
	glutInitWindowSize(default_width, default_height);
	id = glutCreateWindow(title);
	windows[id] = this;
	
	bounds_v.left = (1.0 - cache.ratio) / 2.0;
	bounds_v.right = 1.0 - bounds_v.left;
	cache.x = default_x;
	cache.y = default_y;
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	predraw();
	
	glutReshapeFunc(reshape_func);
	glutKeyboardFunc(keyboard_func);
	glutSpecialFunc(special_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutPassiveMotionFunc(motion_func);
	glutDisplayFunc(display_func);
}

Window::~Window()
{
	glutDestroyWindow(id);
	windows.erase(id);
}

//------------------------------------------------------------------------------

Window::x::operator int() const
{
	return parent.cache.x;
}

Window::y::operator int() const
{
	return parent.cache.y;
}

Window::width::operator int() const
{
	//glutSetWindow(parent.id);
	//return glutGet(GLUT_WINDOW_WIDTH);
	return parent.cache.width;
}

Window::height::operator int() const
{
	//glutSetWindow(parent.id);
	//return glutGet(GLUT_WINDOW_HEIGHT);
	return parent.cache.height;
}

int Window::x::operator =(int const &x)
{
	glutSetWindow(parent.id);
	glutPositionWindow(x, parent.cache.y);
	return parent.cache.x = x;
}

int Window::y::operator =(int const &y)
{
	glutSetWindow(parent.id);
	glutPositionWindow(parent.cache.x, y);
	return parent.cache.y = y;
}

int Window::width::operator =(int const &w)
{
	glutSetWindow(parent.id);
	glutReshapeWindow(w, parent.cache.height);
	parent.cache.ratio = (float) w / (float) parent.cache.height;
	parent.bounds_v.left = (1.0 - parent.cache.ratio) / 2.0;
	parent.bounds_v.right = 1.0 - parent.bounds_v.left;
	return parent.cache.width = w;
}

int Window::height::operator =(int const &h)
{
	glutSetWindow(parent.id);
	glutReshapeWindow(parent.cache.width, h);
	parent.cache.ratio = (float) parent.cache.width / (float) h;
	parent.bounds_v.left = (1.0 - parent.cache.ratio) / 2.0;
	parent.bounds_v.right = 1.0 - parent.bounds_v.left;
	return parent.cache.height = h;
}

//------------------------------------------------------------------------------

void Window::redraw()
{
	glutSetWindow(id);
	glutPostRedisplay();
}

void Window::predraw()
{
	glViewport(0, 0, cache.width, cache.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(bounds.left, bounds.right, bounds.top, bounds.bottom);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::postdraw()
{
	glutSwapBuffers();
}

//------------------------------------------------------------------------------

void Init(int *argc, char *argv[])
{
	glutInit(argc, argv);
}

//------------------------------------------------------------------------------

LoopCallback callback = NULL;

void idle_func()
{
	if (!callback)
		return;
		
	callback();
	glutPostRedisplay();	
}

void Run(LoopCallback cb)
{
	callback = cb;
	glutIdleFunc(idle_func);
	glutMainLoop();
}

//------------------------------------------------------------------------------

} /* namespace GUI */

//..............................................................................
