#include<iostream>
#include "kvstore_api.h"
#include "kvstore.h"

//todo : dupilcate element delete based on timestamp
//todo : choose smaller timestamp when merge

//
//int main()
//{
//	MemCache store("store");
//	//KVStore store("store");
//	//store.reset();
//	//list1:0:a , 3:sss , 4: ssss ,6:ssssss stamp:1
//	//list2:1:s , 2:ss , 4: ~DELETED~ , 6:aaaaaa stamp:2 
//	std::vector<std::list<std::pair<uint64_t, std::string>>> lists;
//
//	std::list<std::pair<uint64_t, std::string>> the_list,list2;
//	the_list.push_back(std::pair<uint64_t, std::string>(0,"a"));
//	the_list.push_back(std::pair<uint64_t, std::string>(3, "sss"));
//	the_list.push_back(std::pair<uint64_t, std::string>(4, "ssss"));
//	the_list.push_back(std::pair<uint64_t, std::string>(6, "ssssss"));
//
//	list2.push_back(std::pair<uint64_t, std::string>(1, "s"));
//	list2.push_back(std::pair<uint64_t, std::string>(2, "ss"));
//	list2.push_back(std::pair<uint64_t, std::string>(4, "~DELETED~"));
//	list2.push_back(std::pair<uint64_t, std::string>(6, "aaaaaa"));
//
//	std::vector<uint64_t> stamp{ 1, 2 };
//	lists.push_back(the_list);
//	lists.push_back(list2);
//	store.merge_sort_test(lists,stamp,0);
//
//	
//	
//	return 0;
//}
