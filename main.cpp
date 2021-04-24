#include<iostream>
#include "MemCache.h"

int main()
{
	//Skiplist skp;
	//SSTable sst;
	//for (int i = 0;i <10;++i)
	//{
	//	skp.put(i, "abc");
	//}
	//skp.showSkipList();
	//sst.write_to_file(&skp);
	//sst.read_cache_from_file("test1.sst");

	//Header h = sst.test_header();
	//std::cout << "header stamp:" << h.stamp << " pair_num:" << h.pair_number << " key_min:" << h.key_min << " key_max:" << h.key_max << std::endl;

	//uint32_t offset;
	//uint32_t data_byte;
	//key_type t = sst.get_offset(0, offset, data_byte);
	//std::cout << "key_type:" << t << " offset:" << offset << " databyte:" << data_byte << std::endl;
	//std::cout << "data from value:"<<sst.read_data_from_file("test1.sst", offset, data_byte,t) << std::endl;
	MemCache mc("");

	mc.reset();

	//for (int j = 0;j < 14;++j)
	//{
	//	
	//	for (int i = 0;i < 10;++i)
	//	{
	//		mc.put(i, "a" + std::to_string(i));
	//	}

	//	mc.put(10, "~DELETED~");

	//	mc.dump2sst();
	//}

	for (int i = 0;i < 10;++i)
	{
		mc.put(i, "a" + std::to_string(i));
	}
	mc.dump2sst();
	

	for (int i = 0;i < 10;++i)
	{
		std::string val;
		mc.get(i,val);
		std::cout << "get:" << val << std::endl;
	}
	for (int i = 20;i < 30;++i)
	{
		std::string val;
		mc.get(i, val);
		std::cout << "get:" << val << std::endl;
	}
	std::string del_val;
	if (!mc.get(10, del_val))
	{
		std::cout << "delete not found" << std::endl;
	}

	//mc.reset();
	

	
	return 0;
}
