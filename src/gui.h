/*******************************************************
 * Graphical user interface -- header file             *
 *                                                     *
 * Authors: Ferry Timmers                              *
 *                                                     *
 * Date: 13:12 5-6-2016                                *
 *                                                     *
 * Description: Graphical representation, window and   *
 *              user input functionality               *
 *******************************************************/

#ifndef _GUI_H
#define _GUI_H

namespace GUI {

//------------------------------------------------------------------------------

struct MouseEvent
{
	int x, y;
	enum Button
	{
		btnLeft = 1,
		btnMiddle = 2,
		btnRight = 4
	} button;
};

struct Rect
{
	float left, right, top, bottom;
};

//------------------------------------------------------------------------------

#define PROPERTY(ptype, type, name) \
struct name { ptype &parent; name(ptype &p) : parent(p) {} \
type operator =(const type &); operator type() const; } name;

class Window
{
public:
	Window(const char *title);
	~Window();
	
	PROPERTY(Window, int, x);
	PROPERTY(Window, int, y);
	PROPERTY(Window, int, width);
	PROPERTY(Window, int, height);
	
	const Rect &bounds;
	
	void redraw();
	
	virtual void resized() {}
	virtual void keypress(unsigned char key) {}
	virtual void mousedown(const MouseEvent &) {}
	virtual void mouseup(const MouseEvent &) {}
	virtual void mousepress(const MouseEvent &) {}
	virtual void mousemove(const MouseEvent &) {}
	
	virtual void predraw();
	virtual void draw() {}
	virtual void postdraw();

private:
	int id;
	static const int buttoncount = 3;
	struct { int state, x, y; } buttons[buttoncount];
	struct { int x, y, width, height; float ratio; } cache;
	Rect bounds_v;
	
	static void reshape_func(int, int);
	static void keyboard_func(unsigned char, int, int);
	static void special_func(int, int, int);
	static void mouse_func(int, int, int, int);
	static void motion_func(int, int);
	static void display_func();
};

//------------------------------------------------------------------------------

typedef void (*LoopCallback)();

void Init(int *argc, char *argv[]);
void Run(LoopCallback);

//------------------------------------------------------------------------------

} /* namespace GUI */

#endif /* _GUI_H */

//..............................................................................
