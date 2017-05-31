#include "TexOGL.h"
#include <PhBase/PhBase.h>
#include <assert.h>

namespace ph
{    
    static void BlobPNGReadFunc(png_structp s_ptr,png_bytep data,png_size_t size)
    {
        IBlob * blob = (IBlob *)png_get_io_ptr(s_ptr);
        PhSizeT ret = blob->Read(data,size);
		if( ret == 0)
			png_error(s_ptr, "pngReaderCallback failed");
    }
	
	GLenum TexFilter2GL( TEX_FILTER filter )
	{
		switch( filter )
		{
			case TEX_FILTER_POINT: return GL_NEAREST;
			case TEX_FILTER_LINEAR: return GL_LINEAR;
			case TEX_FILTER_MIP_POINT: return GL_NEAREST_MIPMAP_NEAREST;
			case TEX_FILTER_MIP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
			default : return GL_NEAREST;
		}
	}
	
	GLenum TexAddress2GL( TEX_ADDRESS addr )
	{
		switch( addr )
		{
			case TEX_ADDRESS_REPEAT: return GL_REPEAT;
			case TEX_ADDRESS_CLAMP: return GL_CLAMP_TO_EDGE;
			case TEX_ADDRESS_MIRROR: return GL_MIRRORED_REPEAT;
			default: return GL_REPEAT;
		}
	}
    
    GLint PixelSize( PIXEL_FORMAT format )
    {
        switch ( format )
        {
			case PIXEL_FORMAT_A8: return 1;
			case PIXEL_FORMAT_RGBA8888: return 4;
			case PIXEL_FORMAT_DEPTH_32: return 4;
			case PIXEL_FORMAT_STENCIL_8: return 1;
			default : return 1;        
        }
    }
	
	GLenum TexFormat2GL( PIXEL_FORMAT format )
	{
		switch( format )
		{
			case PIXEL_FORMAT_A8: return GL_RED;
			case PIXEL_FORMAT_RGBA8888: return GL_RGBA;
			case PIXEL_FORMAT_DEPTH_32: return GL_DEPTH_COMPONENT;
			case PIXEL_FORMAT_STENCIL_8: return GL_STENCIL;
			default : return GL_RGBA;
		}
	}

    TexOGL* TexOGL::CreateTexFromPNG(const char* _image)
    {
		Archive * arch = GetDefArchive();
        IBlob * file = arch->Open( _image );
        if(!file)
            return NULL;
        Image * image = Image::ImageWithPNG( file );
        if(!image)
        {
            file->Release();
            return NULL;
        }
        TexOGL * tex = new TexOGL;
        glGenTextures(1, &tex->tex);
        __gl_check_error__
		glBindTexture(GL_TEXTURE_2D, tex->tex);	__gl_check_error__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , image->nWidth, image->nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pData ); __gl_check_error__
		glGenerateMipmap( GL_TEXTURE_2D ); __gl_check_error__
        tex->desc.format = PIXEL_FORMAT_RGBA8888;
        tex->desc.size.w = image->nWidth;
        tex->desc.size.h = image->nHeight;
        image->Release();
        return tex;
    }
    
    TexOGL * TexOGL::CreateTexFromPNG( const Image * _image )
    {
        TexOGL * tex = new TexOGL;
        glGenTextures(1, &tex->tex);
        __gl_check_error__
		glBindTexture(GL_TEXTURE_2D, tex->tex);	__gl_check_error__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , _image->nWidth, _image->nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );  __gl_check_error__
        for( int h = 0; h < _image->nHeight; ++h )
        {
            glTexSubImage2D( GL_TEXTURE_2D, 0, 0, _image->nHeight - h -1, _image->nWidth, 1, GL_RGBA, GL_UNSIGNED_BYTE, _image->pData + h * _image->nWidth * 4 ); 
        }		
		glGenerateMipmap( GL_TEXTURE_2D ); __gl_check_error__
        tex->desc.format = PIXEL_FORMAT_RGBA8888;
        tex->desc.size.w = _image->nWidth;
        tex->desc.size.h = _image->nHeight;
        return tex;
    }
	
	TexOGL * TexOGL::CreateTexEmpty( PIXEL_FORMAT _format, PhU16 _width, PhU16 _height )
	{
		TexOGL * tex = new TexOGL;
		glGenTextures( 1, &tex->tex );
		__gl_check_error__
		glBindTexture( GL_TEXTURE_2D, tex->tex );
		__gl_check_error__
		GLenum format = TexFormat2GL( _format );
		glTexImage2D(GL_TEXTURE_2D, 0, format , _width, _height, 0, format, GL_UNSIGNED_BYTE, NULL ); __gl_check_error__
        tex->desc.format = _format;	
        tex->desc.size.w = _width;
        tex->desc.size.h = _height;
        return tex;
	}

	TexOGL * TexOGL::CreateTexAlpha8(PhU16 _width, PhU16 _height)
	{
		TexOGL * tex = new TexOGL;
		glGenTextures(1, &tex->tex);
		__gl_check_error__
			glBindTexture(GL_TEXTURE_2D, tex->tex);
		__gl_check_error__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, _width, _height, 0, GL_ALPHA, GL_FLOAT, NULL); __gl_check_error__
			tex->desc.format = PIXEL_FORMAT_A8;
		tex->desc.size.w = _width;
		tex->desc.size.h = _height;
		return tex;
	}

	TexOGL * TexOGL::CreateTexDepthStencil(PhU16 _width, PhU16 _height)
	{
		TexOGL * tex = new TexOGL;
		glGenTextures(1, &tex->tex);
		__gl_check_error__
			glBindTexture(GL_TEXTURE_2D, tex->tex);
		__gl_check_error__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _width, _height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); __gl_check_error__
		tex->desc.format = PIXEL_FORMAT_DEPTH_32;
		tex->desc.size.w = _width;
		tex->desc.size.h = _height;
		return tex;
	}

	TexOGL * TexOGL::CreateTexStencil(PhU16 _width, PhU16 _height)
	{
		TexOGL * tex = new TexOGL;
		glGenTextures(1, &tex->tex);
		__gl_check_error__
		glBindTexture(GL_TEXTURE_2D, tex->tex);
		__gl_check_error__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, _width, _height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_INT, NULL); __gl_check_error__
		tex->desc.format = PIXEL_FORMAT_STENCIL_8;
		tex->desc.size.w = _width;
		tex->desc.size.h = _height;
		return tex;
	}
    
    TexOGL * TexOGL::CreateWhiteTex()
    {
        unsigned int whitebuff[] = {
            0xffffffff,0xffffffff,0xffffffff,0xffffffff,
            0xffffffff,0xffffffff,0xffffffff,0xffffffff,
            0xffffffff,0xffffffff,0xffffffff,0xffffffff,
            0xffffffff,0xffffffff,0xffffffff,0xffffffff,
        };
        TexOGL * tex = new TexOGL;
        glGenTextures(1, &tex->tex);
        __gl_check_error__
		glBindTexture(GL_TEXTURE_2D, tex->tex);	__gl_check_error__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitebuff ); __gl_check_error__
		glGenerateMipmap( GL_TEXTURE_2D ); __gl_check_error__
        tex->desc.format = PIXEL_FORMAT_RGBA8888;
        tex->desc.size.w = 4;
        tex->desc.size.h = 4;
        return tex;
    }
    
    void TexOGL::BufferSubData( int _x, int _y, int _w, int _h, unsigned char * _data )
    {
        glBindTexture( GL_TEXTURE_2D, this->tex );
        for( int i = 0; i < _h; ++i )
        {
            unsigned int dataoffset = PixelSize(this->desc.format) * _w * ( _h - i -1 );
            GLenum format = TexFormat2GL( this->desc.format );
            glTexSubImage2D( 
                GL_TEXTURE_2D, 
                0, 
                _x, 
                _y + i, 
                _w,
                1, 
                format, 
                GL_UNSIGNED_BYTE, 
                _data + dataoffset
            );
        }       
        __gl_check_error__
    }
    
    void TexOGL::GenMipmap()
    {
        glBindTexture( GL_TEXTURE_2D, this->tex);
        glGenerateMipmap( GL_TEXTURE_2D );
    }
    
    void Image::Release()
    {
        delete[] this->pData;
        delete this;
    }
    
    Image * Image::ImageWithPNG(IBlob * _pBlob)
    {
        if(!_pBlob)
            return NULL;
        png_structp s_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
        png_infop   i_ptr = png_create_info_struct(s_ptr);
        setjmp(png_jmpbuf(s_ptr));
        png_set_read_fn(s_ptr, _pBlob,BlobPNGReadFunc);

        png_read_png(s_ptr,i_ptr, PNG_TRANSFORM_EXPAND , 0);
        
        int color_type = i_ptr->color_type;
        //int pixel_depth = i_ptr->pixel_depth;        
        if(i_ptr->bit_depth!=8)
            return NULL;
        if(!i_ptr->channels)
            return NULL;

        Image * pImage = new Image();
        pImage->nHeight = i_ptr->height;
        pImage->nWidth = i_ptr->width;
        // 强制4个字节，32bit~
        unsigned long buff_size = pImage->nHeight * pImage->nWidth * 4;
        png_bytep* rpx_pointers = png_get_rows(s_ptr,i_ptr);
        pImage->pData = new unsigned char[buff_size];
        if(color_type == PNG_COLOR_TYPE_RGBA)
        {
            for(int i = 0;i<pImage->nHeight;i++)
            {
                memcpy(pImage->pData+(pImage->nWidth*4*i),
                       rpx_pointers[i],
                       pImage->nWidth*4
                );
            }
        }
        else if(color_type == PNG_COLOR_TYPE_RGB)
        {
            for(int i = 0;i<pImage->nHeight;i++)
            {
                for(int j = 0;j<pImage->nWidth;j++)
                {
                    unsigned char * pColors = pImage->pData + i*pImage->nWidth*4 + 4 * j;
                    unsigned char * pIn = (unsigned char *)rpx_pointers[i] + 3 * j;
                    pColors[0] = pIn[0];
                    pColors[1] = pIn[1];
                    pColors[2] = pIn[2];
                    pColors[3] = 0xff;
                }
            }
        }
        else
        {
            return NULL;
        }
        png_destroy_read_struct(&s_ptr, &i_ptr, 0);
        return pImage;
    }
	
	const PxTexDesc* TexOGL::GetDesc()
	{
		return &desc;
	}

	void TexOGL::SetSampler(SamplerState& _sampler)
	{
		sampler.SetDesc( _sampler );
	}

	bool TexOGL::Bind( ShaderOGL* _shader, const char* _name )
	{
        __gl_check_error__
		GLint slot = _shader->BindTexture2D( _name );
        __gl_check_error__
		if(slot == 0xff )
		{
			return false;
		}
		glActiveTexture(GL_TEXTURE0+slot);
        __gl_check_error__
		glBindTexture(GL_TEXTURE_2D, this->tex);
        __gl_check_error__
		this->sampler.Bind( slot );
		return true;
	}
    
    void TexOGL::Release()
    {
        glDeleteTextures( 1, & this->tex );
        delete this;
    }
}



