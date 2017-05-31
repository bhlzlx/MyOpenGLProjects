#include "GLView.h"
#include <windows.h>
#include <gl/gl3w.h>
#include <gl/GL.h>
#include <gl/wglext.h>
#include <stdio.h>

namespace ph
{
	typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
	typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats,
		int *piFormats, UINT *nNumFormats);
	typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);


	ViewOGL::ViewOGL()
	{
	}

	ViewOGL::~ViewOGL()
	{
	}


	ViewOGL* ViewOGL::CreateView(void* _wnd)
	{
		ViewOGL * viewOGL = new ViewOGL;

		if (!_wnd)
			return NULL;

		HWND hwnd = (HWND)_wnd;

		HDC hdc = GetDC(hwnd); // Get the device context for our window

		viewOGL->hwnd = hwnd;
		viewOGL->hdc = hdc;

		PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window
		pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels
		pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)
		pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
		pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

		int nPixelFormat = ChoosePixelFormat(hdc, &pfd); // Check if our PFD is valid and get a pixel format back
		if (nPixelFormat == 0) // If it fails
			return NULL;

		BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD
		if (!bResult) // If it fails
			return NULL;

		HGLRC glrc = wglCreateContext(hdc); // Create an OpenGL 2.1 context for our device context
		wglMakeCurrent(hdc, glrc); // Make the OpenGL 2.1 context current and active
		viewOGL->hrc = (void *)glrc;

		// init opengl core profile
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT && wglCreateContextAttribsARB && wglChoosePixelFormatARB)
		{
			const int attribList[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 32,
				WGL_STENCIL_BITS_ARB, 8,

				0,//End
			};

			int pixelFormat;
			UINT numFormats;
			wglChoosePixelFormatARB(hdc, attribList, NULL, 1, &pixelFormat, &numFormats);

			static const int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, OGL_MAJOR,
				WGL_CONTEXT_MINOR_VERSION_ARB, OGL_MINOR,
#if OGL_CORE_PROFILE
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#else
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
#endif
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
				0
			};
			HGLRC core_rc = wglCreateContextAttribsARB(hdc, 0, attribs);
			wglSwapIntervalEXT(1);
			wglMakeCurrent(hdc, core_rc);
			wglDeleteContext(glrc);
		}

		if (gl3wInit()) {
			fprintf(stderr, "failed to initialize OpenGL\n");
		}

		if (!gl3wIsSupported(OGL_MAJOR, OGL_MINOR)) {
			fprintf(stderr, "OpenGL %d.%d not supported\n", OGL_MAJOR, OGL_MINOR);
		}

		printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));

		return viewOGL;
	}

	void ViewOGL::Begin()
	{
		static const GLfloat red[] = { 0.6f, 0.8f, 0.2f, 1.0f };
		GLfloat depth = 1.0f;
		glClearBufferfv(GL_COLOR, 0, red);
		glClearBufferfv(GL_DEPTH, 0, &depth);
	}

	const rect<PhU32> * ViewOGL::GetViewport()
	{
		return &viewport;
	}

	void ViewOGL::End()
	{
		SwapBuffers((HDC)hdc);
	}

	void ViewOGL::Release()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext((HGLRC)this->hrc);
		ReleaseDC((HWND)this->hwnd, (HDC)this->hdc);
		delete this;
	}

	void ViewOGL::Resize(int _w, int _h)
	{
		viewport.right = _w;
		viewport.top = _h;
		glViewport(0, 0, _w, _h);
	}
}