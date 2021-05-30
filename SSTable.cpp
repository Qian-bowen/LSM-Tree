#include "SSTable.h"

uint32_t header_byte = 32;
uint32_t bf_byte = 10240;
uint32_t index_byte = 12;


SSTable::SSTable(std::string sp)
{
	sst_path = sp;
}


/*
return 0 : fail to find key
return 2 : find last key
return 1 : find other key
*/
key_type SSTable::get_offset(uint64_t key, uint32_t& offset, uint32_t& data_byte)
{
	int start = 0,end=index.size()-1;
	while (start <= end)
	{
		int mid = (start + end) / 2;
		uint64_t mid_key=index[mid].key;
		if (key == mid_key)
		{
			offset = index[mid].offset;
			//find data_byte
            if (mid+1 != (int)index.size())
			{
				data_byte = index[mid + 1].offset - offset;
				return ORDIN_KEY;
			}
			else
			{
				return LAST_KEY;
			}
		}
		else if (key < mid_key)
			end = mid - 1;
		else
			start = mid + 1;
	}
	return NONE_KEY;
}

//generate header bloomfilter index
void SSTable::generate_hd_bf_idx(Skiplist* memtable, uint64_t timestamp)
{
	//key:8 byte  offset:4 byte
	uint32_t index_all_byte = memtable->pair_size() * index_byte;
	uint32_t data_offset = header_byte + bf_byte + index_all_byte;

	Node* cur = memtable->get_all_header();
	Node* last_bound;
	while (cur && !cur->isBoundNode())
	{
		//add index
		Index tmp(cur->key, data_offset);
		index.push_back(tmp);
		//add bloomfilter
		bloomfilter.add_key(cur->key);
		//increase offset and use string.size() to get exact size
		data_offset += cur->value.size();
		cur = cur->next;
	}
	last_bound = cur;

	//add header
	header.stamp =timestamp;
	header.pair_number = memtable->pair_size();
    header.key_min = memtable->get_all_header()->key;
    if(last_bound&&last_bound->prev) header.key_max = last_bound->prev->key;
}

void SSTable::generate_hd_bf_idx(std::list<std::pair<uint64_t, std::string>>& table, uint64_t timestamp)
{
	uint32_t index_all_byte =table.size() * index_byte;
	uint32_t data_offset = header_byte + bf_byte + index_all_byte;
	for (auto& kv : table)
	{
		Index tmp(kv.first, data_offset);
		index.push_back(tmp);
		//add bloomfilter
		bloomfilter.add_key(kv.first);
		//increase offset and use string.size() to get exact size
		data_offset += kv.second.size();
	}
	header.stamp = timestamp;
	header.pair_number =table.size();
	header.key_min = table.front().first;
	header.key_max = table.back().first;
}

//TODO:improve write/read efficiency

void SSTable::write_to_file(Skiplist* memtable, uint64_t timestamp)
{
	//generate index from memtable first
	generate_hd_bf_idx(memtable,timestamp);

	std::ofstream outsst(sst_path, std::ios::out | std::ios::binary);
	//write header
	//no alignment in header
	outsst.write(reinterpret_cast<char*>(&header), header_byte);
	//write bloomfilter
	outsst.write(reinterpret_cast<char*>(bloomfilter.get_bf()), bf_byte);
	//write index
	//alignment in index
	for (auto idx : index)
	{
		outsst.write(reinterpret_cast<char*>(&idx.key), sizeof(idx.key));
		outsst.write(reinterpret_cast<char*>(&idx.offset), sizeof(idx.offset));
	}
	//write value
	//cannot use sizeof(string) due to c++ string class implementation
	Node* cur = memtable->get_all_header();
	while (cur && !cur->isBoundNode())
	{
		outsst.write((&cur->value[0]), cur->value.size());
		cur = cur->next;
	}
	outsst.close();
}

void SSTable::write_to_file(std::list<std::pair<uint64_t, std::string>>& table, uint64_t timestamp)
{
	generate_hd_bf_idx(table, timestamp);
	std::ofstream outsst(sst_path, std::ios::out | std::ios::binary);
	outsst.write(reinterpret_cast<char*>(&header), header_byte);
	outsst.write(reinterpret_cast<char*>(bloomfilter.get_bf()), bf_byte);
	for (auto idx : index)
	{
		outsst.write(reinterpret_cast<char*>(&idx.key), sizeof(idx.key));
		outsst.write(reinterpret_cast<char*>(&idx.offset), sizeof(idx.offset));
	}
	for (auto& kv : table)
	{
		outsst.write((&kv.second[0]), kv.second.size());
	}
	outsst.close();
}


void SSTable::read_cache_from_file()
{
	//clear index first
	index.clear();

	std::ifstream insst(sst_path, std::ios::in | std::ios::binary);

	//read header
	insst.read(reinterpret_cast<char*>(&header),header_byte);
	//read bloomfilter
	insst.read(reinterpret_cast<char*>(bloomfilter.get_bf()),bf_byte);
	//read index first offset
	uint64_t first_key;
	uint32_t first_offset;
	insst.read(reinterpret_cast<char*>(&first_key), sizeof(first_key));
	insst.read(reinterpret_cast<char*>(&first_offset), sizeof(first_offset));
	Index tmp_first(first_key, first_offset);
	index.push_back(tmp_first);
	
	uint32_t cur_offset =header_byte+bf_byte+index_byte;

	while(cur_offset<first_offset)
	{

		uint64_t tmp_key;
		uint32_t tmp_offset;
		insst.read(reinterpret_cast<char*>(&tmp_key), sizeof(tmp_key));
		insst.read(reinterpret_cast<char*>(&tmp_offset), sizeof(tmp_offset));
		Index tmp(tmp_key, tmp_offset);
		//std::cout << "tmp_key:" << tmp_key << " tmp_offset:" << tmp_offset << std::endl;
		index.push_back(tmp);
		cur_offset += index_byte;
	}	
	insst.close();
}

std::string SSTable::extract_data_from_stream(std::ifstream& insst, uint32_t of_bgn, uint32_t data_byte, key_type type)
{
	std::string data;
	char* c_data;
	if (LAST_KEY == type)
	{
		insst.seekg(0, std::ios::end);
		uint32_t end_pos = insst.tellg();
		data_byte = end_pos - of_bgn;
	}

	c_data = new char[data_byte + 1];
	c_data[data_byte] = '\0';

	insst.seekg(of_bgn);
	insst.read(c_data, data_byte);

	data.assign(c_data);
	return data;
}

//extract 
std::string SSTable::read_data_from_file(uint32_t of_bgn, uint32_t data_byte, key_type type)
{
	std::ifstream insst(sst_path, std::ios::in | std::ios::binary);
	return extract_data_from_stream(insst,of_bgn,data_byte,type);	
}

void SSTable::read_all_data_from_file(std::list<std::pair<uint64_t, std::string>>& kv_vec)
{
	//if not read cache
	if (index.empty())
	{
		read_cache_from_file();
	}

	std::ifstream insst(sst_path, std::ios::in | std::ios::binary);

	for (auto idx=index.begin();idx!=index.end();++idx)
	{
		std::string str_data;
		uint64_t key = (*idx).key;
		uint32_t offset = (*idx).offset;
		uint32_t offset_next=0;
		//if last key
		if (idx+1!=index.end())
		{
			offset_next = (*(idx + 1)).offset;
			str_data = extract_data_from_stream(insst, offset, offset_next - offset, ORDIN_KEY);		
		}
		else
		{
			str_data = extract_data_from_stream(insst, offset, 0, LAST_KEY);
		}


		kv_vec.push_back(std::pair<uint64_t, std::string>(key,str_data));
	}

	insst.close();
}
