#include "kvstore.h"
#include <string>

KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{
	memCache=new MemCache(dir);
}

KVStore::~KVStore()
{
	delete memCache;
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	memCache->put(key, s);
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	std::string val="";
	if (!memCache->get(key, val))
	{
		val = "";
	}
	
	return val;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	return memCache->del(key);
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	memCache->reset();
}

void KVStore::force_dump_test()
{
	memCache->dump2sst();
}

void KVStore::show_memtable()
{
	memCache->show_memtable();
}

std::string KVStore::get_no_bloom(uint64_t key)
{
	std::string val = "";
	if (!memCache->get_no_bloom(key, val))
	{
		val = "";
	}

	return val;
}

std::string KVStore::get_no_sst(uint64_t key)
{
	std::string val = "";
	if (!memCache->get_no_sst(key, val))
	{
		val = "";
	}

	return val;
}
