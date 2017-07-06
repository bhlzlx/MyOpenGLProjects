#pragma once
#include "TypeDef.h"
#include <memory>
#include <string>

namespace ph
{
	struct IBlob
	{
		virtual PhSizeT 	Size() = 0;
		virtual PhSizeT		Seek(PhU8 _flag, PhU32 _offset) = 0;
		virtual PhSizeT		Read(void* _pOut, PhU32 _nSize) = 0;
		virtual PhSizeT		Write(const void* _pIn, PhU32 _nSize) = 0;
		/* return 0 if success, or return -1 if failed */
		virtual PhSizeT 	Resize(PhSizeT _nSize) = 0;
		virtual bool 		Eof() = 0;
		virtual char* 		GetCurr() = 0;
		virtual char* 		GetBuffer() = 0;
		virtual void 		Release() = 0;
		virtual const char * Filepath() = 0;
		virtual ~IBlob() {};
	};

	class Archive
	{
	private:
		std::string m_root;
	public:
		Archive();
		IBlob * Open(const char * _fp);
		bool Exist(const char * _fp);
		const std::string& GetRoot() const;
		void Init( const char * _root);
		~Archive();
		static std::string FormatFilePath(const std::string& _filepath);
	};

	Archive * GetDefArchive();

	inline std::string GetAssertPath()
	{
		return "/asset/";
	}

}



