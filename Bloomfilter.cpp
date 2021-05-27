#include "Bloomfilter.h"
#include "MurmurHash3.h"

void Bloomfilter::add_key(uint64_t key)
{
	uint32_t hash_val[4];
	MurmurHash3_x64_128(&key, sizeof(key), 1, hash_val);
	for (int i = 0;i < 4;++i)
	{
		//idx is the index of bit to be set to 1
		int idx = hash_val[i] % (BUF_SIZE*8);
		int byte_idx = idx / 8;
		int byte_remain = idx - byte_idx * 8;
		uint8_t mask = 1 << (7 - byte_remain);
		bf[byte_idx] = bf[byte_idx]|mask;
	}
}

/*
return true if key not in bloomfilter
*/
bool Bloomfilter::not_in_bf(uint64_t key)
{
	bool notin = false;
	uint32_t hash_val[4];
	MurmurHash3_x64_128(&key, sizeof(key), 1, hash_val);
	for (int i = 0;i < 4;++i)
	{
		int idx = hash_val[i] % (BUF_SIZE * 8);
		int byte_idx = idx / 8;
		int byte_remain = idx - byte_idx * 8;
		uint8_t mask = 1 << (7 - byte_remain);
		uint8_t maskop = bf[byte_idx] & mask;
		if (maskop==0)
			notin = true;
	}
	return notin;
}