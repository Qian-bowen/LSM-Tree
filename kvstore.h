#pragma once

#include "kvstore_api.h"
#include "MemCache.h"

class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
	MemCache* memCache;

public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;



	void force_dump_test()override;

	void show_memtable()override;

	std::string get_no_bloom(uint64_t key) override;

	std::string get_no_sst(uint64_t key) override;



};
