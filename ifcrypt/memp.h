#pragma once
#include <cstdio>
#include <mutex>
#include <cstdint>
#include <mutex>
#include <cassert>

namespace iflib
{
	class MEMPException
	{
	public:
		enum MemOper
		{
			MemOperAlloc,
			MemOperFree,
		};
		MemOper oper;
	};

	template < class T, uint8_t ALIGN = 3 > // 默认8字节对齐
	class MEMP
	{
	private:
		static const size_t UNITSIZE = (sizeof(T) + (2 << ALIGN) - 1)&~((2 << ALIGN) - 1);
	private:
		std::mutex	mtx;
		//
#pragma pack( push, 4 )
		template< class T >
		struct Unit
		{
			uint8_t		data[UNITSIZE];
			union
			{
				uint32_t flag;
				uint32_t next;
			};
			void MakeAllocated()
			{
				flag |= FLAG_ALLOCATED;
			}
			void MakeFree()
			{
				flag &= FLAG_FREE;
			}
			uint8_t IsAllocated()
			{
				return flag&FLAG_ALLOCATED;
			}
			Unit()
			{
				flag = 0;
			}
			const static int FLAG_ALLOCATED = 0x1;
			const static int FLAG_FREE = ~FLAG_ALLOCATED;
		};
		template< class T >
		struct Block
		{
			uint32_t size;
			uint32_t free;
			uint32_t freeId;
			Block<T>*	 next;
			Unit<T>	 units[0];
			static Block<T> * NewBlock(size_t _nUnit)
			{
				void * ptr = nullptr;
				while (! (ptr = malloc(sizeof(Block<T>) * sizeof(Unit<T>) * _nUnit)) );
				Block<T>* block = (Block<T>*)ptr;
				Unit<T> * unitArry = new(block->units)Unit<T>[_nUnit]; // 调用每个Unit的默认构造函数
				for (size_t i = 0; i < _nUnit-1; ++i)
				{
					block->units[i].next = i + 1;
				}
				block->units[_nUnit - 1].next = 0xffffffff;
				block->free = _nUnit;
				block->freeId = 0;
				block->size = _nUnit;
				block->next = nullptr;
				return block;
			}

			Unit<T>* Alloc()
			{
				if (free)
				{
					--free;					
					Unit<T> * unit = &units[freeId];
					freeId = units[freeId].next;
					unit->MakeAllocated();
					return unit;
				}
				return nullptr;
			}

			bool Free(uint8_t * _ptr)
			{
				if (_ptr < units[0].data || _ptr > units[size-1].data )
				{
					return false;
				}
				size_t loc = (_ptr - &units[0].data[0]) / sizeof(Unit<T>);
				Unit<T>* unit = (Unit<T>*)_ptr;
				if ( (_ptr == (loc * sizeof(Unit<T>) + &units[0].data[0])) && unit->IsAllocated() )
				{
					unit->MakeFree();
					unit->next = freeId;
					freeId = loc;
					++free;
					return true;
				}
				else
				{
					MEMPException except;
					except.oper = MEMPException::MemOperFree;
					throw except;
					return false;
				}
			}
		};
#pragma pack( pop )
	private:
		Block<T>*		head;
		Block<T>*		last;
		uint32_t		blockCount;
		uint32_t		initSize;
		uint32_t		growSize;
	public:
		MEMP( size_t _init, size_t _grow )
		{
			initSize = _init;
			growSize = _grow;
			last = head = Block<T>::NewBlock( _init );
			last->next = nullptr;
			blockCount = 1;
		}

		void Grow()
		{
			last->next = Block<T>::NewBlock(growSize);
			last = last->next;
			++this->blockCount;
		}

		template< class PARAM >
		T* Alloc( PARAM _param )
		{
			std::lock_guard<std::mutex> lock(mtx);

			Block<T> * block = head;
			Unit<T>* ptr = nullptr;
			while (block)
			{
				if (block->free)
				{
					ptr = block->Alloc();
					return new(ptr)T( _param );
					//return (T*)ptr;
				}
				block = block->next;
			}
			Grow();
			ptr = last->Alloc();
			return (T*)ptr;
		}

		template< class PARAM1, class PARAM2 >
		T* Alloc(PARAM1 _param1, PARAM2 _param2 )
		{
			std::lock_guard<std::mutex> lock(mtx);

			Block<T> * block = head;
			Unit<T>* ptr = nullptr;
			while (block)
			{
				if (block->free)
				{
					ptr = block->Alloc();
					return new(ptr)T(_param1, _param2);
					//return (T*)ptr;
				}
				block = block->next;
			}
			Grow();
			ptr = last->Alloc();
			return (T*)ptr;
		}

		template< class PARAM1, class PARAM2, class PARAM3>
		T* Alloc(PARAM1 _param1, PARAM2 _param2, PARAM3 _param3)
		{
			std::lock_guard<std::mutex> lock(mtx);

			Block<T> * block = head;
			Unit<T>* ptr = nullptr;
			while (block)
			{
				if (block->free)
				{
					ptr = block->Alloc();
					return new(ptr)T(_param1, _param2, _param3);
					//return (T*)ptr;
				}
				block = block->next;
			}
			Grow();
			ptr = last->Alloc();
			return (T*)ptr;
		}

		template< class PARAM1, class PARAM2, class PARAM3, class PARAM4>
		T* Alloc(PARAM1 _param1, PARAM2 _param2, PARAM3 _param3, PARAM4 _param4)
		{
			std::lock_guard<std::mutex> lock(mtx);

			Block<T> * block = head;
			Unit<T>* ptr = nullptr;
			while (block)
			{
				if (block->free)
				{
					ptr = block->Alloc();
					return new(ptr)T(_param1, _param2, _param3, _param4);
					//return (T*)ptr;
				}
				block = block->next;
			}
			Grow();
			ptr = last->Alloc();
			return (T*)ptr;
		}

		T* Alloc()
		{
			std::lock_guard<std::mutex> lock(mtx);

			Block<T> * block = head;
			Unit<T>* ptr = nullptr;
			while (block)
			{
				if (block->free)
				{
					ptr = block->Alloc();
					return (T*)ptr;
				}
				block = block->next;
			}
			Grow();
			ptr = last->Alloc();
			return (T*)ptr;
		}

		bool Free( T* _ptr )
		{
			std::lock_guard<std::mutex> lock(mtx);

			Block<T> * block = head;
			while (block)
			{
				if (block->Free((uint8_t*)_ptr))
				{
					return true;
				}
				block = block->next;
			}
			return false;
		}

		~MEMP()
		{
			Block<T> * block = head;
			while (block)
			{
				Block<T> * next = block->next;
				free(block);
				block = next;
			}
		}
	};
}