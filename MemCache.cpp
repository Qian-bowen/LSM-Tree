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


	//if root_path directory not exist, create one
	if (!utils::dirExists(root_path))
	{
		utils::_mkdir(root_path.c_str());
	}

	std::vector<std::string> dir_vec;
	int subdir_num = utils::scanDir(root_path, dir_vec);
	if (!subdir_num)
	{
		cur_timestamp++;
		return;
	}

	//construct sstCache
	for (auto& subdir_path : dir_vec)
	{
		std::list<SSTable*> cur_level;
		std::vector<std::string> sst_vec;
		std::string subdir_path_all = root_path +"/"+ subdir_path;
		int sst_num = utils::scanDir(subdir_path_all, sst_vec);


		for (auto& sst_path : sst_vec)
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

//lists and stamp match in order
void MemCache::multiple_merge(std::vector<SSTable*> vec, int lev)
{
	std::vector<std::list<std::pair<uint64_t, std::string>>> lists;
	std::vector<uint64_t> stamp;
	std::list<std::pair<uint64_t, std::string>> merged_list;
	
	for (auto& sst : vec)
	{
		std::list<std::pair<uint64_t, std::string>> kv_vec;
		sst->read_all_data_from_file(kv_vec);
		lists.push_back(kv_vec);
		stamp.push_back(sst->get_timestamp());

	}
	//sorted list based on timestamp, the sstable with min timestamp is the first one
	//merge sort
	 merge_sort(lists,stamp,lev);

	 ////remove old files after merge
	 for (auto sst : vec)
	 {
		 utils::rmfile((sst)->get_sst_path().data());
		 //free sstable after use
		 delete sst;
	 }
}

/*
read file to memory first, and sort
merge sstable into one
*/
//todo : dupilcate element delete based on timestamp
void MemCache::merge_sort(std::vector<std::list<std::pair<uint64_t,std::string>>>& lists, std::vector<uint64_t>& stamp,int lev)
{
	int max_lev = sstCache.size();
	std::priority_queue<merge_elem> queue;
	std::list<merge_elem> sorted_list;
	uint32_t all_byte = header_byte + bf_byte;

	int stamp_iter = 0;
	for (auto& sst : lists)
	{
		queue.push({sst.begin()->first,stamp[stamp_iter],sst.begin(),sst.end(),sst.begin()->second});
		++stamp_iter;
	}

	while (!queue.empty())
	{
		auto f = queue.top();
		queue.pop();

		//push a new elem to priority queue
		auto new_iter = f.iter;
		new_iter++;
		if (new_iter != f.iter_end)
		{
			uint64_t stamp = f.stamp;
			queue.push({ new_iter->first,stamp,new_iter,f.iter_end,new_iter->second});
		}

		//if the key duplicate, and the elem in the sorted_list timestamp is smaller,just remove it
		if (!sorted_list.empty() && f.key == sorted_list.back().key && f.stamp>sorted_list.back().stamp)
		{
			sorted_list.pop_back();
		}

		uint32_t add_byte= index_byte + sizeof(f.iter->first) + f.iter->second.size();
		if ((all_byte+add_byte) > MT_MAX)
		{
			//chunk the list
			//write to next level
			std::list<std::pair<uint64_t, std::string>> write_list;
			mergelist_to_writelist(sorted_list, write_list);
			write_to_level(write_list, lev + 1);
			sorted_list.clear();
			all_byte = header_byte + bf_byte;
		}

		//check whether need to delete
		if (lev == max_lev && f.value == "~DELETED~")
			continue;

		sorted_list.push_back(f);
		all_byte += add_byte;
	}

	//write the remain list below MT_MAX to level
	std::list<std::pair<uint64_t, std::string>> write_list;
	mergelist_to_writelist(sorted_list, write_list);
	write_to_level(write_list, lev + 1);

	return;
}

std::string MemCache::get_path(int lev)
{
	std::string filename = "/l" + std::to_string(lev) + "-" + std::to_string(cur_timestamp) + ".sst";
	//increase current timestamp
	++cur_timestamp;
	std::string dir_path = root_path + "/level-" + std::to_string(lev);
	std::string file_path = dir_path + filename;

	//if directory not exist
	if (!utils::dirExists(dir_path))
	{
		utils::_mkdir(dir_path.c_str());
	}

	return file_path;
}

void MemCache::mergelist_to_writelist(std::list<merge_elem>& merge_list, std::list<std::pair<uint64_t, std::string>>& write_list)
{
	for (auto& elem : merge_list)
	{
		write_list.push_back(std::make_pair(elem.key,elem.value));
	}
}

void MemCache::write_to_level(std::list<std::pair<uint64_t, std::string>>& table,int lev)
{
	SSTable* sst = new SSTable(get_path(lev));
	//check whether next level exist
	int levels = sstCache.size();
	if (lev + 1 >= levels)
	{
		int level_remain = lev + 1 - levels;
		while (level_remain)
		{
			sstCache.push_back(std::list<SSTable*>());
			--level_remain;
		}
	}
	//add to memory
	std::list<SSTable*>& cur_level = sstCache[lev];
	cur_level.push_back(sst);
	//write to file and construct header in it
	sst->write_to_file(table, cur_timestamp);
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
	
	//write to level-0
	SSTable* sst = new SSTable(get_path(0));
	sst->write_to_file(&memtable, cur_timestamp);
	sstCache.front().push_back(sst);
	//clear memtable after dump
	memtable.clear();
	
	//check merge after dump
	uint32_t max_level = sstCache.size();
	for (uint32_t lev = 0;lev<max_level;++lev)
	{
		auto& level = sstCache[lev];
		uint32_t cur_size = level.size();
		uint32_t max_size = 1 << (lev + 1);

		//enough space
		if (cur_size <= max_size) break;

		//get the file need to merge according to sstCache
		//level-0 should compact all if need to compact
		int flow_num = (lev==0)?cur_size:cur_size-max_size;
		uint64_t min_key = INT64_MAX,max_key=0;
		//add overflow file in this level to merge list
		std::vector<SSTable*> sst_to_merge;
		for (int i = 0;i < flow_num;++i)
		{
			SSTable* sst = level.front();
			//delete sstable in memory
			level.pop_front();
			sst_to_merge.push_back(sst);
			//handle key
			uint64_t cur_min = sst->get_min_key(), cur_max = sst->get_max_key();
			if ( cur_min < min_key)
			{
				min_key = cur_min;
			}
			if (cur_max > max_key)
			{
				max_key = cur_max;
			}
		}

		//add file in next level that overlaps in time to merge list
		//if there is no next level, create next level
		//TODO: using const properly
		if (lev+1!=max_level)
		{
			auto& next_level = sstCache[lev+1];
			for (auto sst_iter=next_level.begin();sst_iter!=next_level.end();++sst_iter)
			{
				uint64_t cur_min = (*sst_iter)->get_min_key(), cur_max = (*sst_iter)->get_max_key();
				bool choice = !(cur_min > max_key || cur_max < min_key);
				if (!(cur_min > max_key || cur_max < min_key))
				{
					sst_to_merge.push_back(*sst_iter);
					next_level.erase(sst_iter);
				}
			}
		}

		//merge lists
		multiple_merge(sst_to_merge,lev);
	}
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
	bool mem_get=memtable.get(key, value);
	//std::cout << "mem get:" << value << std::endl;
	if (mem_get)
		return true;
	//search sstCache
    for (auto& level:sstCache)
    {
		//sort level, sstable timestamp from large to small
		//level.sort([](SSTable* left, SSTable* right) {return left->get_timestamp() < right->get_timestamp();});
        for (auto& sst:level)
        {
			//use bloomfilter to check
			//impossible in sst
			if (key<sst->get_min_key()||key>sst->get_max_key() || sst->not_in_bf(key))
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
					//std::cout << "get value in sst:" << value << std::endl;
					if ("~DELETED~" == value)
						return false;
					else
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
	std::string val;
	if (memtable.get(key,val))
	{
		memtable.del(key);
		memtable.put(key, "~DELETED~");
		return true;
	}
	else
	{
		memtable.put(key, "~DELETED~");
		return false;
	}
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
