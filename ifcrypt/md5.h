#ifndef MD5_H_INCLUDED
#define MD5_H_INCLUDED

#include <cassert>
#include <stdint.h>
#include <string>

namespace md5
{
	const static uint32_t ChainValues[] = 
	{
		0x67452301,
		0xefcdab89,
		0x98badcfe,
		0x10325476,
	};

	struct TurnParam
	{
		uint32_t a, b, c, d;
		uint32_t A, B, C, D;
	};

	const static uint32_t T[] =
	{
		0x00000000,
		0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
		0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
		0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
		0x6b901122,0xfd987193,0xa679438e,0x49b40821,

		0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
		0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
		0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
		0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,

		0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
		0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
		0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
		0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,

		0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
		0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
		0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
		0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
	};

	static const size_t ChunkSize = 64;

	inline uint32_t CLS_32(uint32_t i, uint32_t offset)
	{
		offset = offset & 0x1f; // offset < 32
		return i << offset | i >> (32 - offset);
	}

	inline uint32_t F(uint32_t& x, uint32_t& y, uint32_t& z)
	{
		return (x&y) | ((~x)&z);
	}
	inline uint32_t G(uint32_t& x, uint32_t& y, uint32_t& z)
	{
		return (x&z) | (y&(~z));
	}
	inline uint32_t H(uint32_t& x, uint32_t& y, uint32_t& z)
	{
		return x^y^z;
	}
	inline uint32_t I(uint32_t& x, uint32_t& y, uint32_t& z)
	{
		return y ^ (x | (~z));
	}

	inline void FF(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t& x, uint32_t y, const uint32_t& z)
	{
		a = b + CLS_32(a + F(b, c, d) + x + z, y);
	}
	inline void GG(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t& x, uint32_t y, const uint32_t& z)
	{
		a = b + CLS_32(a + G(b, c, d) + x + z, y);
	}
	inline void HH(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t& x, uint32_t y, const uint32_t& z)
	{
		a = b + CLS_32(a + H(b, c, d) + x + z, y);
	}
	inline void II(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t& x, uint32_t y, const uint32_t& z)
	{
		a = b + CLS_32(a + I(b, c, d) + x + z, y);
	}

	inline void Turn(uint32_t * chunk, TurnParam& _param )
	{
		//
		uint32_t & a = _param.a;
		uint32_t & b = _param.b;
		uint32_t & c = _param.c;
		uint32_t & d = _param.d;
		uint32_t & A = _param.A;
		uint32_t & B = _param.B;
		uint32_t & C = _param.C;
		uint32_t & D = _param.D;

		a = A;
		b = B;
		c = C;
		d = D;

		FF(a, b, c, d, chunk[0], 7, T[1]); FF(d, a, b, c, chunk[1], 12, T[2]); FF(c, d, a, b, chunk[2], 17, T[3]); FF(b, c, d, a, chunk[3], 22, T[4]);
		FF(a, b, c, d, chunk[4], 7, T[5]); FF(d, a, b, c, chunk[5], 12, T[6]); FF(c, d, a, b, chunk[6], 17, T[7]); FF(b, c, d, a, chunk[7], 22, T[8]);
		FF(a, b, c, d, chunk[8], 7, T[9]); FF(d, a, b, c, chunk[9], 12, T[10]); FF(c, d, a, b, chunk[10], 17, T[11]); FF(b, c, d, a, chunk[11], 22, T[12]);
		FF(a, b, c, d, chunk[12], 7, T[13]); FF(d, a, b, c, chunk[13], 12, T[14]); FF(c, d, a, b, chunk[14], 17, T[15]); FF(b, c, d, a, chunk[15], 22, T[16]);
		//
		GG(a, b, c, d, chunk[1], 5, T[17]); GG(d, a, b, c, chunk[6], 9, T[18]); GG(c, d, a, b, chunk[11], 14, T[19]); GG(b, c, d, a, chunk[0], 20, T[20]);
		GG(a, b, c, d, chunk[5], 5, T[21]); GG(d, a, b, c, chunk[10], 9, T[22]); GG(c, d, a, b, chunk[15], 14, T[23]); GG(b, c, d, a, chunk[4], 20, T[24]);
		GG(a, b, c, d, chunk[9], 5, T[25]); GG(d, a, b, c, chunk[14], 9, T[26]); GG(c, d, a, b, chunk[3], 14, T[27]); GG(b, c, d, a, chunk[8], 20, T[28]);
		GG(a, b, c, d, chunk[13], 5, T[29]); GG(d, a, b, c, chunk[2], 9, T[30]); GG(c, d, a, b, chunk[7], 14, T[31]); GG(b, c, d, a, chunk[12], 20, T[32]);
		//
		HH(a, b, c, d, chunk[5], 4, T[33]); HH(d, a, b, c, chunk[8], 11, T[34]); HH(c, d, a, b, chunk[11], 16, T[35]); HH(b, c, d, a, chunk[14], 23, T[36]);
		HH(a, b, c, d, chunk[1], 4, T[37]); HH(d, a, b, c, chunk[4], 11, T[38]); HH(c, d, a, b, chunk[7], 16, T[39]); HH(b, c, d, a, chunk[10], 23, T[40]);
		HH(a, b, c, d, chunk[13], 4, T[41]); HH(d, a, b, c, chunk[0], 11, T[42]); HH(c, d, a, b, chunk[3], 16, T[43]); HH(b, c, d, a, chunk[6], 23, T[44]);
		HH(a, b, c, d, chunk[9], 4, T[45]); HH(d, a, b, c, chunk[12], 11, T[46]); HH(c, d, a, b, chunk[15], 16, T[47]); HH(b, c, d, a, chunk[2], 23, T[48]);
		//
		II(a, b, c, d, chunk[0], 6, T[49]); II(d, a, b, c, chunk[7], 10, T[50]); II(c, d, a, b, chunk[14], 15, T[51]); II(b, c, d, a, chunk[5], 21, T[52]);
		II(a, b, c, d, chunk[12], 6, T[53]); II(d, a, b, c, chunk[3], 10, T[54]); II(c, d, a, b, chunk[10], 15, T[55]); II(b, c, d, a, chunk[1], 21, T[56]);
		II(a, b, c, d, chunk[8], 6, T[57]); II(d, a, b, c, chunk[15], 10, T[58]); II(c, d, a, b, chunk[6], 15, T[59]); II(b, c, d, a, chunk[13], 21, T[60]);
		II(a, b, c, d, chunk[4], 6, T[61]); II(d, a, b, c, chunk[11], 10, T[62]); II(c, d, a, b, chunk[2], 15, T[63]); II(b, c, d, a, chunk[9], 21, T[64]);

		A += a;
		B += b;
		C += c;
		D += d;
	}

	inline void Md5(const uint8_t * _data, size_t _size, uint8_t(&_md5)[16] )
	{
		TurnParam param = {
			0, 0, 0, 0,
			ChainValues[0],ChainValues[1],ChainValues[2],ChainValues[3]
		};
		struct
		{
			union
			{
				uint8_t b[64];
				uint16_t w[32];
				uint32_t dw[16];
				uint64_t ll[8];
			};
		} chunk64[2] = { 0 };
		size_t roundsize = _size & (~63);
		size_t left = _size & 63;
		size_t i = 0;
		for (; i < roundsize; i += 64)
		{
			Turn((uint32_t*)_data + i, param);
		}
		memcpy(&chunk64[0].b[0], _data + i, left);

		if (left < 56)
		{
			chunk64[0].b[left] = 0b10000000; // 如果是 不同大小端的可能不一样 0b00000001
			chunk64[0].ll[7] = (_size) * 8;
			Turn((uint32_t *)&chunk64[0], param);
		}
		else
		{
			chunk64[0].b[left] = 0b1000; // 如果是 不同大小端的可能不一样 0b00000001
			chunk64[1].ll[7] = (128 - left) * 8;
			Turn((uint32_t *)&chunk64[0], param);
			Turn((uint32_t *)&chunk64[1], param);
		}
		memcpy(_md5, &param.A, 16);
	}

	inline void Print( uint8_t(&_md5)[16] )
	{
		for (int i = 0; i<16; i++)
		{
			unsigned char cc = *(((unsigned char *)_md5) + i);
			printf("%02x", cc);
		}
	}
}

#endif