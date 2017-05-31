#pragma once
#include <unordered_map>
#include <PhBase/Archive.h>
#include <graphics/TexOGL.h>
#include <string>
#include <memory>

namespace ph
{
	class TexPool
	{
	private:
		TexPool();
		~TexPool();
	private:
		static std::unordered_map< std::string, TexOGLRef >	TexTable;
		static Archive * Arch;
		static TexOGLRef WhiteTex;
	public:
		static void InitTexPool(Archive * _arch);

		static TexOGLRef Get(const std::string& _filepath);
		static TexOGLRef GetWhite();

		static void KickLazy();
	};
}


