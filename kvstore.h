#pragma once

#include "kvstore_api.h"
#include "Skiplist.h"

//file max size is 2mb
//#define MAX_SIZE (unsigned long long)(2*(1<<20))

class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
	Skiplist skiplist;

public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

};
