#include "ShaderOGL.h"
#include "TexOGL.h"
#include <cassert>

namespace ph
{

	ShaderOGL::ShaderOGL()
	{
		m_vert = m_prog = m_frag = 0;
	}

	ShaderOGL::~ShaderOGL()
	{

	}

	ShaderOGL* ShaderOGL::CreateShader(const char* _vert, const char* _frag)
	{
		if (strlen(_vert) < 16 || strlen(_frag) < 16)
		{
			//PXLOG_I("shader length too short!!! Maybe invalid shader source!");
			assert("invalid shader text!" && 0 );
			return nullptr;
		}

		ShaderOGL * shader = new ShaderOGL;
		shader->m_vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shader->m_vert, 1, (const GLchar * const *)&_vert, NULL);
		glCompileShader(shader->m_vert);
		int compiled;

		glGetShaderiv(shader->m_vert, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint  logSize;
			glGetShaderiv(shader->m_vert, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader->m_vert, logSize, NULL, logMsg);
			//PXLOG_I("shader error : %s", logMsg);
			delete[] logMsg;
			return NULL;
		}


		shader->m_frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shader->m_frag, 1, (const GLchar * const *)&_frag, NULL);
		glCompileShader(shader->m_frag);

		glGetShaderiv(shader->m_frag, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint  logSize;
			glGetShaderiv(shader->m_frag, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader->m_frag, logSize, NULL, logMsg);
			//PXLOG_I("shader error : %s", logMsg);
			delete[] logMsg;
			return NULL;
		}
		shader->m_prog = glCreateProgram();
		glAttachShader(shader->m_prog, shader->m_vert);
		glAttachShader(shader->m_prog, shader->m_frag);
		glLinkProgram(shader->m_prog);

		int linked;
		glGetProgramiv(shader->m_prog, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLint  logSize;
			glGetShaderiv(shader->m_prog, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetProgramInfoLog(shader->m_prog, logSize, NULL, logMsg);
			//PXLOG_I("link error : %s", logMsg);
			delete[] logMsg;
			return NULL;
		}

		glDeleteShader(shader->m_frag);
		glDeleteShader(shader->m_vert);

		shader->m_vecUniform.clear();
		GLint nUniform;
		glGetProgramiv(shader->m_prog, GL_ACTIVE_UNIFORMS, &nUniform);
		GLsizei name_len;
		GLenum type;
		GLint size;
		GLint loc;
		char namebuff[1024];

		PhU32 idxTex = 0;
		int idxSTex = 0;

		for (GLint idx = 0; idx < nUniform; ++idx)
		{
			PhU32& it = idxTex;
			glGetActiveUniform(shader->m_prog, idx, 1024, &name_len, &size, &type, namebuff);
			loc = glGetUniformLocation(shader->m_prog, namebuff);
			uniform_item item;
			item.id = loc;
			item.type = type;
			item.size = size;
			switch (item.type)
			{
			case GL_SAMPLER_2D:
			{
				item.index = it++;
				break;
			}
			case GL_SAMPLER_2D_SHADOW:
			{
				item.index = idxSTex++;
				break;
			}
			default:
			{
				item.index = 0;
				break;
			}
			}

			strcpy(item.name, namebuff);
			std::shared_ptr< SamplerSlot > slot( new SamplerSlot(item.name, item.id, item.index ));
			shader->m_samplerSlots.push_back(slot);
			shader->m_vecUniform.push_back(item);
		}
		return shader;
	}

	GLuint ShaderOGL::BindTexture2D(const char * _uniform)
	{
		std::vector<uniform_item>::iterator iter = m_vecUniform.begin();
		while (iter != m_vecUniform.end())
		{
			if (iter->type == GL_SAMPLER_2D)
			{
				if (strcmp(iter->name, _uniform) == 0)
				{
					glUniform1i(iter->id, iter->index); __gl_check_error__
					return iter->index;
				}
			}
			++iter;
		}
		return 0xff;
	}

	SamplerSlotRef ShaderOGL::GetSamplerSlot(const char * _name)
	{
		for (auto& slot : m_samplerSlots)
		{
			if (slot->slotName == _name)
			{
				return slot;
			}
		}
		return nullptr;
	}

	void ShaderOGL::Release()
	{
		glDeleteProgram(m_prog);
		delete this;
	}

	void ShaderOGL::Bind()
	{
		glUseProgram(m_prog);
	}

	bool ShaderOGL::SetUniform(const char* _name, int _count, void* _data)
	{
		std::vector<uniform_item>::iterator iter = m_vecUniform.begin();
		while (iter != m_vecUniform.end())
		{
			if (strcmp(iter->name, _name) == 0)
			{
				/*
				* GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4,
				* GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4,
				* GL_SAMPLER_2D, or GL_SAMPLER_CUBE
				*/
				switch (iter->type)
				{
				case GL_FLOAT:
				{
					glUniform1fv(iter->id, _count, (float*)_data); return true;
					break;
				}
				case GL_FLOAT_VEC2:
				{
					glUniform2fv(iter->id, _count, (float*)_data); return true;
					break;
				}
				case GL_FLOAT_VEC3:
				{
					glUniform3fv(iter->id, _count, (float*)_data); return true;
					break;
				}
				case GL_FLOAT_VEC4:
				{
					glUniform4fv(iter->id, _count, (float*)_data); return true;
					break;
				}
				case GL_INT:
				{
					glUniform1iv(iter->id, _count, (int*)_data); return true;
					break;
				}
				case GL_INT_VEC2:
				{
					glUniform2iv(iter->id, _count, (int*)_data); return true;
					break;
				}
				case GL_INT_VEC3:
				{
					glUniform3iv(iter->id, _count, (int*)_data); return true;
					break;
				}
				case GL_INT_VEC4:
				{
					glUniform4iv(iter->id, _count, (int*)_data); return true;
					break;
				}
				case GL_FLOAT_MAT2:
				{
					glUniformMatrix2fv(iter->id, _count, GL_FALSE, (float*)_data); return true;
					break;
				}
				case GL_FLOAT_MAT3:
				{
					glUniformMatrix3fv(iter->id, _count, GL_FALSE, (float*)_data); return true;
					break;
				}
				case GL_FLOAT_MAT4:
				{
					glUniformMatrix4fv(iter->id, _count, GL_FALSE, (float*)_data); return true;
					break;
				}
				case GL_SAMPLER_2D:
				{
					glUniform1i(iter->id, *(GLint*)_data); return true;
					break;
				}
				case GL_SAMPLER_2D_SHADOW:
				{
					glUniform1i(iter->id, *(GLint*)_data); return true;
					break;
				}
				}
			}
			iter++;
		}
		return false;
	}


	SamplerSlot::SamplerSlot(const char * _name, GLuint _loc, GLuint _slot)
	{
		slotName = _name;
		uniformLoc = _loc;
		slot = _slot;
	}

	void SamplerSlot::BindTexture(TexOGL* _texture)
	{
		// 把sampler绑定给 GL_TEXTUREX 的绑定槽
		glUniform1i(uniformLoc, slot); __gl_check_error__
		// 激活绑定槽
		glActiveTexture(GL_TEXTURE0 + slot); __gl_check_error__
		// 绑定纹理到绑定槽
		_texture->BindToActivedSlot(); __gl_check_error__
		// 设置纹理采样器状态???????
		glActiveTexture(GL_TEXTURE0 + slot); __gl_check_error__
		glUniform1i(uniformLoc, slot); __gl_check_error__
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TexFilter2GL(samplerState.MinFilter)); __gl_check_error__
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TexFilter2GL(samplerState.MagFilter)); __gl_check_error__
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TexAddress2GL(samplerState.AddressU)); __gl_check_error__
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TexAddress2GL(samplerState.AddressV)); __gl_check_error__
	}

	void SamplerSlot::SetSamplerState(const SamplerState & _samplerState)
	{
		samplerState = _samplerState;
	}

	void SamplerSlot::ApplySamplerState()
	{
	}
}

