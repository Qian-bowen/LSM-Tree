#include "MemCache.h"

MemCache::MemCache(std::string tp)
{
	root_path = tp;
	load_sstCache();
}

MemCache::~MemCache()
{
	//dump remain kv to sstable
    dump2sst();
	//clear ssCache
	clear_sstCache();
}


void MemCache::load_sstCache()
{
	//set init timestamp
	cur_timestamp = -1;

	//init root path
	std::string root_dir_path = "data";
	root_path += root_dir_path;

	//if root_path directory not exist, create one
	if (!utils::dirExists(root_path))
	{
		utils::_mkdir(root_dir_path.c_str());
	}

	std::vector<std::string> dir_vec;
	int subdir_num = utils::scanDir(root_path, dir_vec);
	if (!subdir_num) return;

	//construct sstCache
	for (auto subdir_path : dir_vec)
	{
		std::list<SSTable*> cur_level;
		std::vector<std::string> sst_vec;
		std::string subdir_path_all = root_path +"/"+ subdir_path;
		int sst_num = utils::scanDir(subdir_path_all, sst_vec);

		for (auto sst_path : sst_vec)
		{
			std::string sst_path_all = subdir_path_all + "/" + sst_path;
			SSTable* sst = new SSTable(sst_path_all);
			sst->read_cache_from_file();
			cur_level.push_back(sst);

			//find the largest
			uint64_t tmp_timestamp = sst->get_timestamp();
			if (tmp_timestamp > cur_timestamp)
				cur_timestamp = tmp_timestamp;
		}
		sstCache.push_back(cur_level);
	}
	//increase timestamp
	cur_timestamp++;
}

/*
dump memtable to sstable
*/
void MemCache::dump2sst()
{
	//if nothing to dump , just return
	if (0 == memtable.size()) return;

	//create level0 if sstCache is empty
    if (sstCache.empty())
    {
        std::list<SSTable*> level0;
        sstCache.push_back(level0);
    }
	
	//TODO:write to level-0 and merge recursively
	uint32_t lev = 0;
	for (auto& level : sstCache)
	{
		uint32_t cur_size = level.size();
		uint32_t max_size = 1 << (lev + 1);
		if (cur_size < max_size)
		{	
			//write test
			std::string filename = "/l"+std::to_string(lev)+"-" + std::to_string(cur_timestamp) + ".sst";
			//increase current timestamp
			++cur_timestamp;
			std::string dir_path = root_path + "/level-" + std::to_string(lev);
			std::string file_path = dir_path + filename;

			if (!utils::dirExists(dir_path))
				utils::_mkdir(dir_path.c_str());

			SSTable* sst = new SSTable(file_path);
			sst->write_to_file(&memtable,cur_timestamp);
			level.push_back(sst);
			break;//no return!
		}
		else
		{
			//add next level to sstCache
			std::list<SSTable*> next_level;
			sstCache.push_back(next_level);
			//create next level directory
			std::string dir_path = root_path + "/level-" + std::to_string(lev+1);
			if (!utils::dirExists(dir_path))
				utils::_mkdir(dir_path.c_str());
		}
		++lev;
	}

	//clear memtable after dump
	memtable.clear();
}

bool MemCache::put(uint64_t key, std::string value)
{
	/*
	uint32_t header_byte = 32;
	uint32_t bf_byte = 10240;
	uint32_t index_byte = 12;
	*/
	uint32_t cur_byte = header_byte + bf_byte + index_byte * memtable.pair_size() + memtable.ubyte_size();
	uint32_t all_byte = cur_byte + sizeof(key) + value.size();
	if (all_byte > MT_MAX)
	{
		dump2sst();
	}
	return memtable.put(key,value);
}

bool MemCache::get(uint64_t key, std::string& value)
{
	//search memtable first
	if (memtable.get(key, value))
		return true;
	//search sstCache
    for (auto level:sstCache)
    {
        for (auto sst:level)
        {
			//use bloomfilter to check
			//impossible in sst
			if (sst->not_in_bf(key))
			{
				continue;
			}
			//probable in sst
			else
			{
				uint32_t offset, data_byte;
				key_type kt = sst->get_offset(key, offset, data_byte);
				if (NONE_KEY == kt)
                    continue;
				else
				{
					value=sst->read_data_from_file(offset, data_byte, kt);
                    if ("~DELETED~" == value)
                        continue;
					return true;
				}
            }
        }
	}
	return false;
}

bool MemCache::del(uint64_t key)
{
	//do not delete directly add new one
	return put(key, "~DELETED~");
}

void MemCache::clear_sstCache()
{
	for (auto& level : sstCache)
	{
		for (auto& sst : level)
		{
			delete sst;
		}
		level.clear();
	}
	sstCache.clear();
}

/*
delete all sst file and directory
*/
void MemCache::reset()
{
	int lev = 0;
	for (auto level : sstCache)
	{
		for (auto sst : level)
		{
			std::string sst_path = sst->get_sst_path();
			if (0 != utils::rmfile(sst_path.c_str()))
			{
				throw(FAIL_DEL_SST);
			}
		}
		//remove corresponding level directory
		std::string level_path = root_path+"/level-" + std::to_string(lev);
		if (utils::dirExists(level_path))
		{
			utils::rmdir(level_path.c_str());
		}
		lev++;
	}
	//clear ssCache
	clear_sstCache();
	//clear timestamp to zero
	cur_timestamp = 0;
}
