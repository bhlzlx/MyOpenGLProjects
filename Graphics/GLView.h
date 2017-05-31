#include <PhBase/PhBase.h>

#define OGL_MAJOR 4
#define OGL_MINOR 1
#define OGL_CORE_PROFILE 1

namespace ph
{
	class ViewOGL
	{
	public:
		ViewOGL();
		~ViewOGL();

		void Begin();
		void End();

		void Resize(int _w, int _h);
		void Release();
	private:
		void * hwnd; // windows hwnd
		void * hdc;  // HDC
		void * hrc;  // OpenGL context

		rect<PhU32> viewport;
	public:
		const rect<PhU32>* GetViewport();
		static ViewOGL * CreateView(void * wnd);
	};
}


