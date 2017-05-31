#pragma once
#include "GraphicDesc.h"
#include "TexOGL.h"
#include <gl/gl3w.h>
#include <gl/GL.h>
#include <cassert>

namespace ph
{
	struct RenderPassDesc
	{
		PIXEL_FORMAT	format;
		PhU16			width;
		PhU16			height;
	};

	inline GLuint Format2OGL(PhU32 _format)
	{
		switch (_format)
		{
		case PIXEL_FORMAT_A8:	return GL_R8;
		case PIXEL_FORMAT_RGBA8888:	return GL_RGBA;
		case PIXEL_FORMAT_DEPTH_STENCIL: return GL_DEPTH_STENCIL;
		default: return GL_RGBA8;
		}
	}

	struct ColorAttachmentOGL
	{
		TexOGL* texture;
		ColorAttachmentOGL()
		{
			texture = 0;
		}

		~ColorAttachmentOGL()
		{
			if (texture)
			{
				texture->Release();
			}
		}

		static ColorAttachmentOGL* CreateColorAttachment(const RenderPassDesc& _desc)
		{
			ColorAttachmentOGL * ca = new ColorAttachmentOGL();
			ca->texture = TexOGL::CreateTexEmpty(_desc.format, _desc.width, _desc.height); __gl_check_error__
			glBindTexture(GL_TEXTURE_2D, 0);
			return ca;
		}
	};

	struct DepthStencilAttachmentOGL
	{
		TexOGL* texture;
		DepthStencilAttachmentOGL()
		{
			texture = 0;
		}

		~DepthStencilAttachmentOGL()
		{
			if (texture)
			{
				texture->Release();
			}
		}

		static DepthStencilAttachmentOGL* CreateDepthStencilAttachment(const RenderPassDesc& _desc)
		{
			DepthStencilAttachmentOGL * dsa = new DepthStencilAttachmentOGL();
			dsa->texture = TexOGL::CreateTexDepthStencil(_desc.width, _desc.height);
			return dsa;
		}
	};

	struct StencilAttachmentOGL
	{
		TexOGL* texture;
		StencilAttachmentOGL()
		{
			texture = 0;
		}

		~StencilAttachmentOGL()
		{
			if (texture)
			{
				texture->Release();
			}
		}

		static StencilAttachmentOGL* CreateStencilAttachment(const RenderPassDesc& _desc)
		{
			StencilAttachmentOGL * sa = new StencilAttachmentOGL();
			sa->texture = TexOGL::CreateTexStencil(_desc.width, _desc.height);
			return sa;
		}
	};

	class RenderPassOGL
	{
	private:
		GLuint							framebuffer;
		ColorAttachmentOGL*				color;
		DepthStencilAttachmentOGL *		depthStencil;
		RenderPassDesc					desc;
	public:
		void Begin()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
		}
		void End()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		TexOGL * GetTexture()
		{
			return color->texture;
		}

		TexOGL * GetDepthStencil()
		{
			return depthStencil->texture;
		}

		void Resize(PhU16 _widht, PhU16 _height)
		{
			if (this->desc.width != _widht || this->desc.height != _height)
			{
				delete color;
				delete depthStencil;
				RenderPassDesc dsc;
				dsc.format = desc.format;
				dsc.width = _widht;
				dsc.height = _height;
				desc = dsc;
				ColorAttachmentOGL * ca = ColorAttachmentOGL::CreateColorAttachment(dsc);
				if (!ca)
					return;
				DepthStencilAttachmentOGL * dsa = DepthStencilAttachmentOGL::CreateDepthStencilAttachment(dsc);
				if (!dsa)
					return;
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ca->texture->tex, 0); __gl_check_error__
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsa->texture->tex, 0); __gl_check_error__
					GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
				assert(error == GL_FRAMEBUFFER_COMPLETE);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				color = ca;
				depthStencil = dsa;
			}
		}

		RenderPassOGL() {}
		static RenderPassOGL * CreateRenderPass(const RenderPassDesc& _desc)
		{
			GLuint fb;
			glGenFramebuffers(1, &fb); __gl_check_error__
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
			ColorAttachmentOGL * ca = ColorAttachmentOGL::CreateColorAttachment(_desc);
			if (!ca)
				return nullptr;
			DepthStencilAttachmentOGL * dsa = DepthStencilAttachmentOGL::CreateDepthStencilAttachment(_desc);
			if (!dsa)
				return nullptr;

			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ca->texture->tex, 0); __gl_check_error__
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsa->texture->tex, 0); __gl_check_error__
			GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
			assert( error == GL_FRAMEBUFFER_COMPLETE );
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			RenderPassOGL * pass = new RenderPassOGL();
			pass->color = ca;
			pass->depthStencil = dsa;
			pass->framebuffer = fb;
			pass->desc = _desc;
			return pass;
		}

		~RenderPassOGL()
		{
			if (color)
				delete color;
			if (depthStencil)
				delete depthStencil;
			if (framebuffer)
				glDeleteFramebuffers(1, &framebuffer);
		}
	};
}



