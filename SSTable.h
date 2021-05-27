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
	void generate_hd_bf_idx(std::list<std::pair<uint64_t, std::string>>& table, uint64_t timestamp);

	std::string extract_data_from_stream(std::ifstream& insst, uint32_t of_bgn, uint32_t data_byte, key_type type);

public:
	SSTable(std::string sp);
	~SSTable(){}

	key_type get_offset(uint64_t key, uint32_t& offset, uint32_t& data_byte);
	void write_to_file(Skiplist* memtable, uint64_t timestamp);
	void write_to_file(std::list<std::pair<uint64_t, std::string>>& table, uint64_t timestamp);
	

	void read_cache_from_file();
	std::string read_data_from_file(uint32_t of_bgn, uint32_t data_byte,key_type type);
	void read_all_data_from_file(std::list<std::pair<uint64_t,std::string>>& kv_vec);

	bool not_in_bf(uint64_t key) { return bloomfilter.not_in_bf(key);}
	std::string get_sst_path() { return sst_path; }

	uint64_t get_timestamp() { return header.stamp; }
	uint64_t get_min_key() { return header.key_min; }
	uint64_t get_max_key() { return header.key_max; }

	Index get_index(int idx) { return index[idx]; }
	int get_index_size() { return index.size(); }

	Header test_header() { return header; }
};



