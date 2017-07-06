#include "Archive.h"
#include <cassert>
namespace ph
{
	struct Blob :public IBlob
	{
		std::string m_filepath;
		// Ö¸Õë
		char*		m_pData;
		char*		m_pCurr;
		char*		m_pEnd;
		// ÈÝÁ¿
		PhSizeT		m_nCapacity;

		Blob()
		{
			this->m_nCapacity = 0;
			this->m_pCurr = this->m_pData = this->m_pEnd = NULL;
		}

		Blob(const void * _pSrc, PhU32 & _nSize)
		{
			m_pData = (char*)malloc(_nSize + 1);
			memcpy(m_pData, _pSrc, _nSize);
			m_pData[_nSize] = 0x00;
			m_pCurr = m_pData;
			m_pEnd = m_pData + _nSize;
			m_nCapacity = _nSize;
		}

		Blob(const PhU32& _nSize)
		{
			m_pData = (char *)malloc(_nSize + 1);
			memset(m_pData, 0, _nSize + 1);
			//m_pData[_nSize] = 0;
			m_pCurr = m_pData;
			m_pEnd = m_pData + _nSize;
			m_nCapacity = _nSize;
		}

		PhSizeT Size()
		{
			return m_nCapacity;
		}

		PhSizeT Seek(PhU8 _flag, PhU32 _offset)
		{
			switch (_flag)
			{
			case SEEK_SET:
				m_pCurr = m_pData + _offset;
				break;
			case SEEK_CUR:
				m_pCurr += _offset;
				break;
			case SEEK_END:
				m_pCurr = m_pEnd + _offset;
				break;
			}

			if ((m_pEnd - m_pCurr) < 0)
			{
				m_pCurr = m_pEnd;
			}
			else if (m_pCurr < m_pData)
			{
				m_pCurr = m_pData;
			}

			return 1;
		}

		PhSizeT Read( void* _pOut, PhU32 _nSize)
		{
			PhU32 sizeLeft = m_pEnd - m_pCurr;
			if (!sizeLeft)
			{
				return 0;
			}
			if (_nSize > sizeLeft)
			{
				_nSize = sizeLeft;
			}
			memcpy(_pOut, m_pCurr, _nSize);
			m_pCurr += _nSize;
			return _nSize;
		}

		PhSizeT Resize(PhSizeT _nSize)
		{
			assert(static_cast<PhSizeT>(_nSize) > this->m_nCapacity);
			PhU32 currOffset = m_pCurr - m_pData;
			this->m_pData = (char *)realloc(m_pData, _nSize + 1);
			assert(m_pData);

			m_pData[_nSize] = 0x0;
			m_nCapacity = _nSize;
			m_pCurr = m_pData + currOffset;
			m_pEnd = m_pData + _nSize;

			return 0;
		}

		PhSizeT Write(const void* _pIn, PhU32 _nSize)
		{
			PhSizeT sizeLeft = m_pEnd - m_pCurr;
			while (sizeLeft < _nSize)
			{
				Resize(m_nCapacity * 2);
				sizeLeft = m_pEnd - m_pCurr;
			}
			memcpy(m_pCurr, _pIn, _nSize);
			m_pCurr += _nSize;
			return _nSize;
		}

		bool Eof()
		{
			if (m_pCurr >= m_pEnd)
			{
				return true;
			}
			return false;
		}

		char* GetCurr()
		{
			return m_pCurr;
		}

		char* GetBuffer()
		{
			return m_pData;
		}

		void Release()
		{
			free(m_pData);
			this->m_nCapacity = 0;
			this->m_pCurr = this->m_pData = this->m_pEnd = NULL;
			delete this;
		}

		const char * Filepath()
		{
			return m_filepath.c_str();
		}

		~Blob()
		{
			return;
		}
	};

	Archive::Archive()
	{

	}

	Archive::~Archive()
	{
	}

	std::string Archive::FormatFilePath(const std::string & _filepath)
	{
		int nSec = 0;
		std::string curSec;
		std::string fpath;
		const char * ptr = _filepath.c_str();
		while (*ptr != 0)
		{
			if (*ptr == '\\' || *ptr == '/')
			{
				if (curSec.length() > 0)
				{
					if (curSec == ".") {}
					else if (curSec == ".." && nSec >= 2)
					{
						int secleft = 2;
						while (!fpath.empty() && secleft == 0)
						{
							if (fpath.back() == '\\' || fpath.back() == '/')
							{
								--secleft;
							}
							fpath.pop_back();
						}
					}
					else
					{
						if( !fpath.empty() )
							fpath.push_back('/');
						fpath.append(curSec);
						++nSec;
					}
					curSec.clear();
				}
			}
			else
			{
				curSec.push_back( *ptr );
				if (*ptr == ':')
				{
					--nSec;
				}
			}
			++ptr;
		}
		if (curSec.length() > 0)
		{
			if (!fpath.empty())
				fpath.push_back('/');
			fpath.append(curSec);
		}
		return fpath;
	}

	IBlob * Archive::Open(const char * _fp)
	{
		std::string validPath = _fp;
		FILE * fp = nullptr;
		fp = fopen( _fp, "rb");
		if (!fp)
		{
			validPath = m_root + validPath;
			fp = fopen(validPath.c_str(), "rb");
		}		
		if (!fp)
		{
			return nullptr;
		}
		PhU32 set = ftell(fp);
		fseek(fp, 0, SEEK_END);
		PhU32 end = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		Blob * blob = new Blob( end - set);
		blob->m_filepath = validPath;
		char * buffer = blob->GetBuffer();
		long long ret  = fread(buffer, 1, blob->Size(), fp);
		return blob;
	}

	bool Archive::Exist(const char * _fp)
	{
		std::string fullpath = m_root + _fp;
		FILE* fp = fopen(fullpath.c_str(), "rb");
		if (!fp)
		{
			return false;
		}
		fclose(fp);
		return true;
	}

	const std::string& Archive::GetRoot() const
	{
		return m_root;
	}

	void Archive::Init(const char * _root)
	{
		this->m_root = _root;
	}
	Archive * GetDefArchive()
	{
		static Archive arch;
		return &arch;
	}
}