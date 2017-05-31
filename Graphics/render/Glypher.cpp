#include "Glypher.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <string>
#include <PhBase/Archive.h>
#include <cassert>

namespace ph
{
	/*const GRDRef ph::Glyph::Get(const GD & _desc)
	{
		
	}*/

	Glypher::Glypher()
	{
	}

	Glypher::~Glypher()
	{
		FT_Done_FreeType(library);
		//FT_Done_Face(face);
	}

	bool Glypher::Init(const char * _fontFile)
	{
		fontBlob = GetDefArchive()->Open( _fontFile );
		heightTable.resize(256, 0);
		if (!fontBlob)
			return false;

		int error = FT_Init_FreeType(&library);
		if (error)
		{
			return NULL;
		}

		error = FT_New_Memory_Face(
			library,
			(FT_Bytes)fontBlob->GetBuffer(),
			fontBlob->Size(),
			0,
			&face);
		if (error)
		{
			return NULL;
		}
		error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);

		Canvas * pCanvas = new Canvas(512, 512);
		vecCanvas.push_back(pCanvas);

		return true;
	}

	GRDRef Glypher::Get(const GD & _desc)
	{
		auto findRet = glyphMap.find(_desc);
		if (findRet != glyphMap.end())
		{
			return findRet->second;
		}
		// 生成
		// 从文件里读取
		//int dpi = 96;
		//int point_size = dpi * _desc.bound / 72;

		int error = FT_Set_Char_Size(face, _desc.bound << 6, 0, 72, 72);

		int glyphIndex = FT_Get_Char_Index(face, _desc.unicode);
		if (glyphIndex == 0)
		{
			glyphIndex = FT_Get_Char_Index(face, u'*');
		}
		error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		assert(error == 0);
		if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		}
		assert(error == 0);

		short bitmap_width = face->glyph->bitmap.width;
		short bitmap_height = face->glyph->bitmap.rows;
		unsigned char * glyphbitmap = (unsigned char *)face->glyph->bitmap.buffer;

		//int bboxLeft = face->bbox.xMin;
		//int bboxBottom = face->bbox.yMin;
		//int offsetY = (face->glyph->metrics.vertAdvance >> 6) - bitmap_height;// (face->glyph->metrics.horiBearingY >> 6) - (face->bbox.yMin) - bitmap_height;
		//int offsetY = (face->glyph->advance.x>>6) - face->glyph->bitmap_top - face->glyph->bitmap.rows;
		int offsetY = (face->glyph->metrics.horiBearingY >> 6) - bitmap_height - (face->size->metrics.descender >> 6);
		int offsetX = face->glyph->metrics.horiBearingX >> 6;
		int glyph_width = face->glyph->metrics.horiAdvance >> 6;

		////////////////////////////////////////////
		std::shared_ptr<GlyphRenderDesc> desc = std::shared_ptr<GlyphRenderDesc>(new GlyphRenderDesc);
		desc->penx = offsetX;
		/*desc->peny = offsetY;
		desc->bmpHeight = bitmap_height;*/
		desc->peny = offsetY;
		desc->bmpHeight = bitmap_height;
		desc->bmpWidth = bitmap_width;
		desc->width = glyph_width;
		desc->height = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
		//desc->height = offsetY + desc->bmpHeight;
		for (size_t i = 0; i < vecCanvas.size(); ++i)
		{
			bool ret = vecCanvas[i]->Alloc(desc->bmpWidth, desc->bmpHeight, desc->canvasX, desc->canvasY, desc->subcanvasId, desc->unitId);
			if (ret)
			{
				desc->canvasId = i;
				desc->tcxMin = (float)desc->canvasX / (float)vecCanvas[i]->width;
				desc->tcxMax = (float)(desc->canvasX + desc->bmpWidth) / (float)vecCanvas[i]->width;
				desc->tcyMin = (float)desc->canvasY / (float)vecCanvas[i]->height;
				desc->tcyMax = (float)(desc->canvasY + desc->bmpHeight) / (float)vecCanvas[i]->height;
				vecCanvas[i]->WritePixel(desc->canvasX, desc->canvasY, desc->bmpWidth, desc->bmpHeight, glyphbitmap);
				glyphMap[_desc] = desc;
				heightTable[_desc.bound] = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
				return desc;
				break;
			}
		}

		// 说明没有空闲的，可用的
		// KickLazy

		for (auto it = glyphMap.begin(); it != glyphMap.end();)
		{
			if (it->second.use_count() == 1)
			{
				bool freeRet = vecCanvas[it->second->canvasId]->vecSubCanvas[it->second->subcanvasId].Free(it->second->unitId);
				assert(freeRet);
				it = glyphMap.erase( it );
			}
			else
			{
				it++;
			}
		}
		// 再分配一次
		for (size_t i = 0; i < vecCanvas.size(); ++i)
		{
			bool ret = vecCanvas[i]->Alloc(desc->bmpWidth, desc->bmpHeight, desc->canvasX, desc->canvasY, desc->subcanvasId, desc->unitId);
			if (ret)
			{
				desc->canvasId = i;
				desc->tcxMin = (float)desc->canvasX / (float)vecCanvas[i]->width;
				desc->tcxMax = (float)(desc->canvasX + desc->bmpWidth) / (float)vecCanvas[i]->width;
				desc->tcyMin = (float)desc->canvasY / (float)vecCanvas[i]->height;
				desc->tcyMax = (float)(desc->canvasY + desc->bmpHeight) / (float)vecCanvas[i]->height;
				vecCanvas[i]->WritePixel(desc->canvasX, desc->canvasY, desc->bmpWidth, desc->bmpHeight, glyphbitmap);
				glyphMap[_desc] = desc;
				heightTable[_desc.bound] = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;
				return desc;
				break;
			}
		}
		// 还不行？？！！！
		// 分配新Canvas
		// canvas 都不够用了，再添加一个
		Canvas * pCanvas = new Canvas(512, 512);
		bool allocRet = pCanvas->Alloc(desc->bmpWidth, desc->bmpHeight, desc->canvasX, desc->canvasY, desc->subcanvasId, desc->unitId);
		assert(allocRet);
		vecCanvas.push_back(pCanvas);
		desc->canvasId = vecCanvas.size() - 1;
		desc->tcxMin = (float)desc->canvasX / (float)pCanvas->width;
		desc->tcxMax = (float)(desc->canvasX + desc->bmpWidth) / (float)pCanvas->width;
		desc->tcyMin = (float)desc->canvasY / (float)pCanvas->height;
		desc->tcyMax = (float)(desc->canvasY + desc->bmpHeight) / (float)pCanvas->height;
		pCanvas->WritePixel(desc->canvasX, desc->canvasY, desc->bmpWidth, desc->bmpHeight, glyphbitmap);
		glyphMap[_desc] = desc;
		heightTable[_desc.bound] = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;

		return desc;
	}

	PhU16 Glypher::FaceHeight(PhU16 _bound)
	{
		assert(_bound < 0xff);
		PhU16 height = heightTable[_bound];
		if (height != 0) return height;
		int error = FT_Set_Char_Size(face, _bound << 6, _bound << 6, 72, 72);
		heightTable[_bound] = face->bbox.yMax - face->bbox.yMin;
		return heightTable[_bound];
	}

	TexOGLRef Glypher::GetCanvasTexRef(PhSizeT _index)
	{
		if( _index < vecCanvas.size() )
			return vecCanvas[_index]->texture;
		return nullptr;
	}

}