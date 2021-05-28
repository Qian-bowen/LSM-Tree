//#include<iostream>
//#include "kvstore_api.h"
//#include "kvstore.h"
//
////todo : dupilcate element delete based on timestamp
////todo : choose smaller timestamp when merge
//
//
//int main()
//{
//
//	KVStore store("store");
//	store.reset();
//	//Skiplist store;
//
//	//int scale[2] = { 512,1024 * 64 };
//	//for (int k = 0;k < 2;++k)
//	//{
//	//	int limit = scale[k];
//	//	for (int i = 0; i < limit; ++i) {
//	//		store.put(i, std::string(i, 's'));
//	//		std::string get_val = store.get(i);
//	//		bool result = (std::string(i, 's') == get_val);
//	//		if (result == false)
//	//		{
//	//			std::cout << "result expected:" << std::string(i, 's') << "  got:" << get_val << std::endl;
//	//		}
//	//	}
//	//}
//
//	for (int i = 1; i < 200; ++i) {
//		store.put(i, std::string(i, 's'));
//	}
//	store.force_dump_test();
//
//	store.del(100);
//
//	std::cout<<"get:"<<store.get(100)<<std::endl;
//
//	
//	return 0;
//}
