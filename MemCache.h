#pragma once
#include "SSTable.h"
#include "utils.h"
#include <list>
#include <filesystem>
#include <queue>

//memtable max size is 2MB
#define MT_MAX 2*(1<<20)

/*
file hierarchy
©À©¤©¤ data/
©¦   ©À©¤©¤ level-0/
|	|		©À©¤©¤l0-(file index).sst
|	|		©À©¤©¤l0-2.sst
|	|		...
©¦   ©À©¤©¤ level-1
©¦   ©À©¤©¤ level-2
©¦   ...
*/

enum CACHE_ERROR{IVD_PATH,FAIL_DEL_SST,FAIL_DEL_DIR};

struct merge_elem {
	uint64_t key;
	uint64_t stamp;
	std::list<std::pair<uint64_t, std::string>>::iterator iter;
	std::list<std::pair<uint64_t, std::string>>::iterator iter_end;
	std::string value;
	bool operator<(const merge_elem& rhs)const {
		return key > rhs.key;
	}
};

class MemCache{
	Skiplist memtable;
	std::vector<std::list<SSTable*>> sstCache;
	std::string root_path;/*root path is empty or end with '/' */
	uint64_t cur_timestamp;
	int cur_file_idx;

	void load_sstCache();
	void clear_sstCache();

	void write_to_level(std::list<std::pair<uint64_t, std::string>>& table,int lev,uint64_t stamp);
	std::string get_path(int lev);
	void mergelist_to_writelist(std::list<merge_elem>& merge_list, std::list<std::pair<uint64_t, std::string>>& write_list);

	void multiple_merge(std::vector<SSTable*> vec,int lev);
	void merge_sort(std::vector<std::list<std::pair<uint64_t, std::string>>>& lists, std::vector<uint64_t>& stamp,int lev);

	

public:
	MemCache(std::string tp);
	~MemCache();

	bool put(uint64_t key, std::string value);
	bool get(uint64_t key, std::string& value);
	bool del(uint64_t key);
	void reset();

	void dump2sst();//should be private, public for test
	
	//test
	void show_memtable() { memtable.showSkipList(); }
	void merge_sort_test(std::vector<std::list<std::pair<uint64_t, std::string>>>& lists, std::vector<uint64_t>& stamp, int lev)
	{ 
		merge_sort(lists,stamp,lev); 
	}
	bool get_no_bloom(uint64_t key, std::string& value);
	bool get_no_sst(uint64_t key, std::string& value);
};
