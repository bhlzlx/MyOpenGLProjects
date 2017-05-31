#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <graphics/GLView.h>

class Application
{
public:
	enum eKeyEvent
	{
		eKeyDown,
		eKeyUp
	};
	enum eMouseButton
	{
		LButtonMouse,
		RButtonMouse,
		MButtonMouse,
		MouseButtonNone
	};
	enum eMouseEvent
	{
		MouseMove,
		MouseDown,
		MouseUp
	};
public:
    Application();
    ~Application();
private:
    ph::ViewOGL * 			view;
public:
    void Start( void * _hwnd );
    void End();
    void OnRender( unsigned long _tick );
	void OnResize( int _w, int _h );
	void OnKeyEvent( unsigned char _key, eKeyEvent _event );
	void OnMouseEvent( eMouseButton _bt , eMouseEvent _event, int _x, int _y );
};

#endif // __APPLICATION_H__
