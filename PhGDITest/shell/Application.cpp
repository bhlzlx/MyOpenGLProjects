#include "Application.h"
#include <assert.h>
#include <PhBase/Archive.h>
#include <PhBase/PhCamera.h>
#include <windows.h>
#include <gl/gl3w.h>
#include <gl/GL.h>
#include <graphics/RenderPass.h>
#include <res/TexPool.h>
#include <render/GUIRender.h>
#include <render/UIWidget.h>
#include <render/Glypher.h>
#include <res/ObjModelLoader.h>
#include <render/Basic3DRender.h>
#include <render/GridRender.h>
#include <glm/gtc/matrix_transform.hpp>

typedef ph::UIWidget Widget;
typedef ph::GUIRender Render;
typedef ph::Basic3DRender ModelRender;
typedef ph::GDIRenderRectExt RRC;
typedef ph::TexOGLRef Texture;
typedef ph::GridRender GridRender;

Widget  tankWidget;
Render* render;
ModelRender * modelRender;
GridRender * gridRender;
Texture gTex;

ph::PhCamera	camera;

glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;
glm::mat4x4 matModelScale;

ph::BasicModelRef model;

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
	modelRender = new ph::Basic3DRender();
	modelRender->Init(arch);
	modelRender->SetLight(glm::vec4(100, 100, -100, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0));

	gridRender = new ph::GridRender();
	gridRender->Init(arch, 0.5, 30);
	
	
	//model = ph::CreateModel3D("./Teapot.obj");
	//model = ph::CreateModel3D("./Low-Poly-Racing-Car.obj");
	//model = ph::CreateModel3D("./Mickey_Mouse.obj");
	model = ph::CreateModel3D("./low-poly-mill.obj");
	ph::AABB& aabb = model->aabb;

	glm::vec3 modelCenter((aabb.min.x + aabb.max.x) / 2, (aabb.min.y + aabb.max.y) / 2, (aabb.min.z + aabb.max.z) / 2);
	float blm = (aabb.max.x - aabb.min.x) > (aabb.max.y - aabb.min.y) ? (aabb.max.x - aabb.min.x) : (aabb.max.y - aabb.min.y);
	blm = blm > (aabb.max.z - aabb.min.z) ? blm : (aabb.max.z - aabb.min.z);

	matModelScale =  glm::translate(glm::scale<float>(glm::mat4x4(), glm::vec3(2.0f / blm, 2.0f / blm, 2.0f / blm)), -modelCenter) ;

	//matModelScale = model.
	gTex = ph::TexPool::Get("system/texture/axe.png");
	camera.SetMatView(glm::vec3(0, 5, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	
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

//	matProj = glm::perspectiveFov<float>(120, _w, _h, 0.1f, 500.0f);
//	matView = glm::lookAt(glm::vec3(0, 5, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	camera.Perspective(120, float(_w)/float(_h), 0.1f, 500.0f);
	//matModel = glm::mat4x4();
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
	camera.Tick();

	matProj = camera.GetProjMatrix();
	matView = camera.GetViewMatrix();

	static float angle = 0.0f;
	angle += 0.05f;
	matModel = glm::rotate( matModelScale, angle, glm::vec3(0, 1, 0));
	view->Begin(); __gl_check_error__;
	gridRender->Begin(matView, matProj);
	gridRender->Draw();
	render->Begin();
	render->Draw(&tankWidget);
	modelRender->Begin(matView, matProj);
	modelRender->Draw(matModel, model, ph::eRenderModeFill);
	modelRender->End();
    view->End();
}

void Application::OnKeyEvent( unsigned char _key, eKeyEvent _event )
{
	if( _event == eKeyDown )
	{
		
		switch( _key )
		{
			case 'W':
			{
				camera.Forward(0.1);
				break;
			}
			case 'S':
			{
				camera.Backward(0.1);
				break;
			}
			case 'A':
			{
				camera.Leftward(0.1);
				break;
			}
			case 'D':
			{
				camera.Rightward(0.1);
				break;
			}
			case 'I':
			{
				camera.RotateAxisX(-5);
				break;
			}
			case 'K':
			{
				camera.RotateAxisX(5);
				break;
			}
			case 'J':
			{
				camera.RotateAxisY(-5);
				break;
			}
			case 'L':
			{
				camera.RotateAxisY(5);
				break;
			}
			default :
			break;
		}
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

	static bool MouseDownState = false;

	static glm::vec2 lastPoint;
	
	switch( _event )
	{
		case MouseDown:
		{
			//ui->on_touch_event( _x, view->GetViewport()->top-_y, uiv2::Touch::touch_event_down, 0);
			MouseDownState = true;
			lastPoint.x = _x;
			lastPoint.y = _y;
			break;
		}
		case MouseUp:
		{
			//ui->on_touch_event(_x, view->GetViewport()->top - _y, uiv2::Touch::touch_event_up, 0);
			MouseDownState = false;
			break;
		}
		case MouseMove:
		{
			//ui->on_touch_event(_x, view->GetViewport()->top - _y, uiv2::Touch::touch_event_move, 0);
			if (MouseDownState)
			{
				camera.RotateAxisY((_x - lastPoint.x) / 10.0f);
				camera.RotateAxisX((_y - lastPoint.y) / 10.0f);
				lastPoint.x = _x;
				lastPoint.y = _y;
			}
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

