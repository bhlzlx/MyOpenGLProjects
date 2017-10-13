// aes.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdint.h>
#include <vector>
#include <array>
#include "aes.h"
#include "base64.h"
#include "md5.h"
#include "memp.h"
#include <thread>
#include <mutex>

/*

int main()
{
	const char datasource[] = \
		u8"We always knew our daughter Kendall was going be a performer of some sort. She entertained people in our small town by putting on shows on our front porch when she was only three or four. Blonde-haired, blue-eyed, and beautiful, she sang like a little angel and mesmerized1 everyone.When Kendall was five, we began to notice that she was blinking a lot and clearing her throat frequently.";
	// ase 128bit cbc pkcs5 padding
	struct 
	{
		union
		{
			aes::AesChunk chunk;
			uint8_t md5[16];
		};
	}CBCIV = {0}, MD5 = { 0 };

	aes::AesChunk aesKey[11];

	uint8_t password[] = "phantom lancer";
	uint8_t iv[] = "hello,world!";

	// create aes key & cbc iv vector
	md5::Md5(password, sizeof(password), aesKey[0].data);
	md5::Md5(iv, sizeof(iv), CBCIV.md5);
	// extend aes key
	ExtendKey(aesKey);
	// 
	std::vector<aes::AesChunk> encryptData;
	std::vector<aes::AesChunk> decryptData;
	// encrypt & decrypt
	size_t encryptSize = aes::Encrypt_CBC_PKCS5Padding(datasource, sizeof(datasource), &CBCIV.chunk, aesKey, encryptData);
	size_t decryptSize = aes::Decrypt_CBC_PKCS5Padding(encryptData.data(), encryptSize, &CBCIV.chunk, aesKey, decryptData);

	// base64 test
	std::string base64;
	base64::Base64Encode((uint8_t*)datasource, sizeof(datasource)-1, base64);

	md5::Md5((uint8_t*)"hello,world!", 12, MD5.md5);
	md5::Print(MD5.md5);

	return 0;
}

*/

struct Object
{
	int type;
	Object(int _type)
	{
		type = _type;
	}
};

iflib::MEMP<aes::AesChunk> AesChunkPool(256, 128);

void alloc_thread(int id) 
{
	std::vector< aes::AesChunk * > chunks;
	for (size_t i = 0; i < 256; ++i)
	{
		aes::AesChunk * chunk = AesChunkPool.Alloc();
		chunks.push_back(chunk);
	}
	for (auto& ptr : chunks)
	{
		AesChunkPool.Free(ptr);
	}
	chunks.clear();
	for (size_t i = 0; i < 256; ++i)
	{
		aes::AesChunk * chunk = AesChunkPool.Alloc();
		chunks.push_back(chunk);
	}
	for (auto& ptr : chunks)
	{
		AesChunkPool.Free(ptr);
	}
	AesChunkPool.Free( (aes::AesChunk*)((uint8_t*)chunks[0] + 4) );
}

int main()
{
	std::thread threads[10];
	for (int i = 0; i<10; ++i)
		threads[i] = std::thread(alloc_thread, i + 1);
	for (auto& th : threads) th.join();

	return 0;
}