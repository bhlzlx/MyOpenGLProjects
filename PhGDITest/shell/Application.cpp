#include "Application.h"
#include <assert.h>
#include <PhBase/Archive.h>
#include <windows.h>
#include <gl/gl3w.h>
#include <gl/GL.h>
#include <graphics/RenderPass.h>
#include <res/TexPool.h>
#include <render/GUIRender.h>
#include <render/UIWidget.h>
#include <render/Glypher.h>

typedef ph::UIWidget Widget;
typedef ph::GUIRender Render;
typedef ph::GDIRenderRectExt RRC;
typedef ph::TexOGLRef Texture;

Widget  tankWidget;
Render* render;
Texture gTex;



Application::Application()
{    
}

Application::~Application()
{    
}

void Application::Start(void* _hwnd)
{
	view = ph::ViewOGL::CreateView(_hwnd);
	char fullpath[256];
	GetModuleFileNameA(NULL, fullpath, 256);
	char * ptr = &fullpath[strlen(fullpath) - 1];

	while (*(--ptr) != '\\') {}

	std::string rootdir;
	rootdir.assign(&fullpath[0], ptr + 1);
	rootdir.append("/asset/");
	ph::Archive * arch = ph::GetDefArchive();
	arch->Init(rootdir.c_str());
	ph::TexPool::InitTexPool(arch);

	/*script::GetScriptEngine()->Init();
	uiv2::regist_to_script();
	*/
	render = ph::GUIRender::GetInstance(arch);

	gTex = ph::TexPool::Get("system/texture/axe.png");
}

void Application::OnResize( int _w, int _h )
{
    if( _w < 4)
    {
        _w = 4;
    }
    if( _h < 4)
    {
        _h = 4;
    }

	_w = (_w >> 2) << 2;
	_h = (_h >> 2) << 2;

	view->Resize(_w, _h);
	ph::GUIRender::GUIViewport(_w, _h);
}

void Application::End()
{
    view->Release();
}   

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

void Application::OnRender(unsigned long _tick)
{
	view->Begin(); __gl_check_error__
	render->Begin();
	render->Draw(&tankWidget);
    view->End();
}

void Application::OnKeyEvent( unsigned char _key, eKeyEvent _event )
{
	ph::UIWidget::DefScissor(0, 1024, 0, 1024);
	if( _event == eKeyDown )
	{
		RRC rrc;
		rrc.MtcXMin = 0.0f; rrc.MtcXMax = 1.0f; rrc.MtcYMin = 0.0f; rrc.MtcYMax = 1.0f;
		tankWidget.Begin();
		rrc.Color = 0xffffffff; rrc.Gray = 0;
		rrc.PosXMin = 0; rrc.PosXMax = 64; rrc.PosYMin = 0.0f; rrc.PosYMax = 64;

		ph::rect<float> tc;
		tc.bottom = 0.0f; tc.top = 32.0f;
		
		switch( _key )
		{
			case 'W':
			{
				tc.left = 64.0f; tc.right = 96.0f;
				break;
			}
			case 'S':
			{
				tc.left = 96.0f; tc.right = 128.0f;
				break;
			}
			case 'A':
			{
				tc.left = 0.0f; tc.right = 32.0f;
				break;
			}
			case 'D':
			{
				tc.left = 32.0f; tc.right = 64.0f;
				break;
			}
			default :
			break;
		}

		rrc.TcXMin = tc.left / 205.0f;
		rrc.TcXMax = tc.right / 205.0f;
		rrc.TcYMin = tc.bottom / 115.0f;
		rrc.TcYMax = tc.top / 115.0f;
		rrc.Color = 0xffffffff;

		tankWidget.Build(rrc, gTex, nullptr, 0);

		char16_t message[] = u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789中国智造，慧及全球。";

		ph::UIText text;
		text.x = 0;
		text.y = 0;
		text.size = 24;
		text.length = sizeof(message) / sizeof(char16_t) - 1;
		text.text = message;
		text.color = 0xff0000ff;
		text.charGap = 0.0f;
		tankWidget.Build(text, 1);
		tankWidget.End();
	}
}

void Application::OnMouseEvent( eMouseButton _bt , eMouseEvent _event, int _x, int _y  )
{
	static int lx = 0;
	static int ly = 0;
	
	static int dx;
	static int dy;
	
	dx = _x - lx;
	dy = _y - ly;
	ly = _y;
	lx = _x;
	
	switch( _event )
	{
		case MouseDown:
		{
			//ui->on_touch_event( _x, view->GetViewport()->top-_y, uiv2::Touch::touch_event_down, 0);
			break;
		}
		case MouseUp:
		{
			//ui->on_touch_event(_x, view->GetViewport()->top - _y, uiv2::Touch::touch_event_up, 0);
			break;
		}
		case MouseMove:
		{
			//ui->on_touch_event(_x, view->GetViewport()->top - _y, uiv2::Touch::touch_event_move, 0);
			break;
		}
	}
}

namespace uiv2
{
	void on_textinput_begin(int _x, int _y, const char * _text)
	{

	}
}

