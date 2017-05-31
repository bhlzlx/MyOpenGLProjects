#include "TexPool.h"

namespace ph
{
	std::unordered_map< std::string, std::shared_ptr<TexOGL> >	TexPool::TexTable;
	Archive * TexPool::Arch;
	std::shared_ptr<TexOGL> TexPool::WhiteTex;

	TexPool::TexPool()
	{
	}

	TexPool::~TexPool()
	{
	}

	void TexPool::InitTexPool(Archive * _arch)
	{
		// 初始化纹理池
		Arch = _arch;
		// 初始化默认纹理（白图）
		TexOGL * whiteTex = TexOGL::CreateWhiteTex();
		WhiteTex = std::move(std::shared_ptr<TexOGL>(whiteTex, [&](TexOGL * _tex)
		{
			_tex->Release();
		}));
	}

	std::shared_ptr<TexOGL> TexPool::Get(const std::string & _filepath)
	{
		auto ret = TexTable.find(_filepath);
		if (ret != TexTable.end())
		{
			return ret->second;
		}
		IBlob * blob = Arch->Open(_filepath.c_str());
		if (!blob)
			return WhiteTex;
		ph::Image* image = ph::Image::ImageWithPNG(blob);
		if (!image)
		{
			blob->Release();
			return WhiteTex;
		}
		TexOGL * texture = ph::TexOGL::CreateTexFromPNG(image);
		image->Release();
		blob->Release();
		std::shared_ptr<TexOGL> ptr(texture);
		TexTable[_filepath] = ptr;
		return std::move(ptr);
	}

	TexOGLRef TexPool::GetWhite()
	{
		return WhiteTex;
	}

	inline void TexPool::KickLazy()
	{
		std::vector< std::string > lazyKeys;
		for (auto it : TexTable)
		{
			if (it.second.use_count() == 1)
			{
				lazyKeys.push_back(it.first);
			}
		}
		for (auto it : lazyKeys)
		{
			TexTable.erase(it);
		}
	}
}

