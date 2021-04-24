#pragma once

#include<vector>
#include<fstream>
#include "Bloomfilter.h"
#include "Skiplist.h"

enum key_type{NONE_KEY,ORDIN_KEY,LAST_KEY};

extern uint32_t header_byte;
extern uint32_t bf_byte;
extern uint32_t index_byte;

struct Header {
	uint64_t stamp;
	uint64_t pair_number;
	uint64_t key_min;
	uint64_t key_max;
};

struct Index {
	uint64_t key;
	uint32_t offset;
	Index(uint64_t k, uint32_t of):key(k),offset(of){}
};

class SSTable {
	Header header;
	Bloomfilter bloomfilter;
	std::vector<Index> index;

	std::string sst_path;
	
	void generate_hd_bf_idx(Skiplist* memtable,uint64_t timestamp);

public:
	SSTable(std::string sp);
	key_type get_offset(uint64_t key, uint32_t& offset, uint32_t& data_byte);
	void write_to_file(Skiplist* memtable, uint64_t timestamp);
	void read_cache_from_file();
	std::string read_data_from_file(uint32_t of_bgn, uint32_t data_byte,key_type type);
	bool not_in_bf(uint64_t key) { return bloomfilter.not_in_bf(key);}
	std::string get_sst_path() { return sst_path; }
	uint64_t get_timestamp() { return header.stamp; }

	Header test_header() { return header; }
};



