#pragma once
#include <stdint.h>

typedef uint8_t			PhU8;
typedef uint16_t		PhU16;
typedef uint32_t		PhU32;
typedef uint64_t		PhU64;
typedef size_t			PhSizeT;

typedef int8_t			PhI8;
typedef int16_t			PhI16;
typedef int32_t			PhI32;
typedef int64_t			PhI64;

namespace ph
{
	template < class T>
	struct point
	{
		T x;
		T y;
		point()
		{
			x = y = 0;
		}
		point(T _x, T _y) :(x)_x, (y)_y
		{}
		void set(T _x, T _y)
		{
			x = _x; y = _y;
		}
	};

	template < class T>
	struct size
	{
		T w;
		T h;
		size()
		{
			w = h = 0;
		}
		size(T _w, T _h)
		{
			w = _w;
			h = _h;
		}
		void set(T _w, T _h)
		{
			w = _w; h = _h;
		}
	};

	template < class T>
	struct rect
	{
		T left, right;
		T bottom, top;
		rect()
		{
			left = right = bottom = top = 0;
		}
		rect(T l, T r, T b, T t)
		{
			left = l; right = r; bottom = b; top = t;
		}
		void set(T _left, T _right, T _bottom, T _top)
		{
			left = _left; right = _right; top = _top; bottom = _bottom;
		}

		bool operator==(const rect<T>& _rc)const
		{
			return (left == _rc.left&& right == _rc.right && bottom == _rc.bottom && top == _rc.top);
		}

		// 如果没有相交，返回false
		bool clip(rect& _other, rect& _result) const
		{
			_result.left = this->left > _other.left ? this->left : _other.left;
			_result.right = this->right < _other.right ? this->right : _other.right;
			_result.bottom = this->bottom > _other.bottom ? this->bottom : _other.bottom;
			_result.top = this->top < _other.top ? this->top : _other.top;
			if (_result.bottom >= _result.top || _result.right <= _result.left)
			{
				return false;
			}
			return true;
		}

		bool hit(const point<T>& _pt) const
		{
			if (_pt.x < this->left || _pt.x > this->right || _pt.y < this->bottom || _pt.y > this->top)
			{
				return false;
			}
			return true;
		}
	};

	template < class T >
	struct area
	{
		T x;T y;
		T w;T h;
		area()
		{
			x = y = w = h = 0;
		}
		area(T _x, T _y, T _w, T _h):( x)_x, (y)_y, (w)_w,(h)_h
		{}
		void set(T _x, T _y, T _w, T _h)
		{   
			x = _x;
			y = _y;
			w = _w;
			h = _h;
		}
	};

	struct vec4
	{
		float x, y, z, w;
		vec4()
		{
			x = y = z = w = 0.0f;
		}
		vec4(float _x, float _y, float _z, float _w)
		{
			x = _x; y = _y; z = _z; w = _w;
		}
	};

	struct vec3
	{
		float x, y, z;
		vec3()
		{
			x = y = z = 0.0f;
		}
		vec3(float _x, float _y, float _z)
		{
			x = _x; y = _y; z = _z;
		}
	};

	struct vec2
	{
		float x, y;
		vec2()
		{
			x = y = 0.0f;
		}
		vec2(float _x, float _y)
		{
			x = _x; y = _y;
		}
	};

}
