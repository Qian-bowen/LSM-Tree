#pragma once
#include "SSTable.h"
#include "utils.h"
#include <list>
#include <filesystem>

//memtable max size is 2MB
#define MT_MAX 2*(1<<20)

/*
file hierarchy
©À©¤©¤ data/
|	©À©¤©¤ timestamp.stp
©¦   ©À©¤©¤ level-0/
|	|		©À©¤©¤l0-(time stamp).sst
|	|		©À©¤©¤l0-2.sst
|	|		...
©¦   ©À©¤©¤ level-1
©¦   ©À©¤©¤ level-2
©¦   ...
*/

enum CACHE_ERROR{IVD_PATH,FAIL_DEL_SST,FAIL_DEL_DIR};

class MemCache {
	Skiplist memtable;
	std::list<std::list<SSTable*>> sstCache;
	std::string root_path;/*root path is empty or end with '/' */
	uint64_t cur_timestamp;

	void load_sstCache();
	void clear_sstCache();

	void merge_level() {};
	void real_del();//delete in last level when merge

public:
	MemCache() {};
	MemCache(std::string tp);
	~MemCache();

	void dump2sst();//should be public

	bool put(uint64_t key, std::string value);
	bool get(uint64_t key, std::string& value);
	bool del(uint64_t key);
	void reset();
};
