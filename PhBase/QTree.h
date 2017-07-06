#pragma once
#include "TypeDef.h"
#include <vector>
#include <memory>
#include <stack>

namespace ph
{
	template< class T, class RT, const PhU8 MAXLVL = 4 >
	struct QTree
	{
		typedef std::shared_ptr<T> ElementType;

		struct QTreeCell
		{
			PhU32		lvl;
			rect<RT>	rc;
			QTreeCell*	x1y1;
			QTreeCell*	x2y1;
			QTreeCell*	x1y2;
			QTreeCell*	x2y2;
			PhU32		index;
		};

		QTreeCell m_rootCell;
		std::vector< std::vector< ElementType > > dataArray;

		void Delete(const ElementType& _ele, const rect<RT>& _rc)
		{
			std::vector<size_t> collidedIncides;
			std::stack< QTreeCell* > stk;
			stk.push(&m_rootCell);

			rect<RT> clipRt;

			while (!stk.empty())
			{
				QTreeCell * c = stk.top();
				stk.pop();
				if (c->rc.clip(_rc, clipRt))
				{
					if (c->lvl == MAXLVL - 1)
					{
						collidedIncides.push_back(c->index);
					}
					else
					{
						stk.push(c->x1y1);
						stk.push(c->x2y1);
						stk.push(c->x1y2);
						stk.push(c->x2y2);
					}
				}
			}

			for (auto& index : collidedIncides)
			{
				for (auto& iter = dataArray[index].begin(); iter != dataArray.end();)
				{
					if (*iter == _ele)
					{
						dataArray[index].erase( iter );
						break;
					}
				}
			}
		}

		void Insert(const ElementType& _ele, const rect<RT>& _rc)
		{
			std::vector<size_t> collidedIncides;
			std::stack< QTreeCell* > stk;
			stk.push(&m_rootCell);

			rect<RT> clipRt;

			while (!stk.empty())
			{
				QTreeCell * c = stk.top();
				stk.pop();
				if (c->rc.clip(_rc, clipRt))
				{
					if (c->lvl == MAXLVL - 1)
					{
						collidedIncides.push_back(c->index);
					}
					else
					{
						stk.push(c->x1y1);
						stk.push(c->x2y1);
						stk.push(c->x1y2);
						stk.push(c->x2y2);
					}
				}
			}

			for (auto& index : collidedIncides)
			{
				dataArray[index].push_back( _ele );
			}
		}

		void EnumElements(const rect<RT>& _rc, std::vector< ElementType >& _vecElements )
		{
			static std::vector<size_t> collidedIncides;
			static std::stack< QTreeCell* > stk;
			collidedIncides.clear();

			stk.push(&m_rootCell);

			rect<RT> clipRt;

			while (!stk.empty())
			{
				QTreeCell * c = stk.top();
				stk.pop();
				if (c->rc.clip(_rc, clipRt))
				{
					if (c->lvl == MAXLVL - 1)
					{
						collidedIncides.push_back(c->index);
					}
					else
					{
						stk.push(c->x1y1);
						stk.push(c->x2y1);
						stk.push(c->x1y2);
						stk.push(c->x2y2);
					}
				}
			}
			_vecElements.clear();
			for (auto& index : collidedIncides)
			{
				for (auto& ele : dataArray[index])
				{
					_vecElements.push_back(ele);
				}
			}
			// ÅÅÐò
			std::sort(_vecElements.begin(), _vecElements.end(), [](ElementType& _a, ElementType& _b) {
				return _a.get() > _b.get();
			});
			
			std::vector< ElementType >::iterator last = _vecElements.end();
			for (auto& iter = _vecElements.begin(); iter != _vecElements.end();)
			{
				if (iter == last)
				{
					iter = _vecElements.erase( iter );
				}
				else
				{
					last = iter;
					++iter;
				}

			}
		}

		void Init(const rect<RT>& _rc)
		{
			m_rootCell = {
				0, 
				_rc,
				nullptr,nullptr,nullptr,nullptr,0xffffffff
			};

			std::stack< QTreeCell* > stk;
			stk.push( &m_rootCell );
			while (!stk.empty())
			{
				QTreeCell * c = stk.top();
				stk.pop();
				if (c->lvl < MAXLVL - 1 )
				{
					c->x1y1 = new QTreeCell();
					*c->x1y1 = {
						c->lvl + 1,
						{ c->rc.left, (c->rc.left + c->rc.right) / 2, c->rc.bottom, (c->rc.bottom + c->rc.top) / 2 },
						nullptr,nullptr,nullptr,nullptr,0xffffffff
					};
					c->x2y1 = new QTreeCell();
					*c->x2y1 = {
						c->lvl + 1,
						{ (c->rc.left + c->rc.right) / 2, c->rc.right, c->rc.bottom, (c->rc.bottom + c->rc.top) / 2 },
						nullptr,nullptr,nullptr,nullptr,0xffffffff
					};
					c->x1y2 = new QTreeCell();
					*c->x1y2 = {
						c->lvl + 1,
						{ c->rc.left, (c->rc.left + c->rc.right) / 2, (c->rc.bottom + c->rc.top) / 2, c->rc.top},
						nullptr,nullptr,nullptr,nullptr,0xffffffff
					};
					c->x2y2 = new QTreeCell();
					*c->x2y2 = {
						c->lvl + 1,
						{ (c->rc.left + c->rc.right) / 2, c->rc.right, (c->rc.bottom + c->rc.top) / 2, c->rc.top},
						nullptr,nullptr,nullptr,nullptr,0xffffffff
					};
					stk.push(c->x1y1);
					stk.push(c->x1y2);
					stk.push(c->x2y1);
					stk.push(c->x2y2);
				}
				else
				{
					c->index = dataArray.size();
					dataArray.push_back( std::move(std::vector<ElementType>()) );
				}
			}
		}
	};
};

