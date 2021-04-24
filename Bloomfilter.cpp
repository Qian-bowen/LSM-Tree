#include "Bloomfilter.h"
#include "MurmurHash3.h"

void Bloomfilter::add_key(uint64_t key)
{
	uint32_t hash_val[4];
	MurmurHash3_x64_128(&key, sizeof(key), 1, hash_val);
	for (int i = 0;i < 4;++i)
	{
		int idx = hash_val[i] % 10240;
		bf[idx] = 1;
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
		uint32_t idx = hash_val[i] % 10240;
		if (0 == bf[idx])
			notin = true;
	}
	return notin;
}