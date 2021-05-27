#include<iostream>
#include "kvstore_api.h"
#include "kvstore.h"


//void regular_test(uint64_t max)
//{
//	uint64_t i;
//
//	// Test a single key
//	EXPECT(not_found, store.get(1));
//	store.put(1, "SE");
//	EXPECT("SE", store.get(1));
//	EXPECT(true, store.del(1));
//	EXPECT(not_found, store.get(1));
//	EXPECT(false, store.del(1));
//
//
//	// Test multiple key-value pairs
//	for (i = 0; i < max; ++i) {
//		store.put(i, std::string(i + 1, 's'));
//		EXPECT(std::string(i + 1, 's'), store.get(i));
//	}
//
//	// Test after all insertions
//	for (i = 0; i < max; ++i)
//		EXPECT(std::string(i + 1, 's'), store.get(i));
//
//	// Test deletions
//	for (i = 0; i < max; i += 2)
//		EXPECT(true, store.del(i));
//
//	for (i = 0; i < max; ++i)
//		EXPECT((i & 1) ? std::string(i + 1, 's') : not_found,
//			store.get(i));
//
//	for (i = 1; i < max; ++i)
//		EXPECT(i & 1, store.del(i));
//
//}

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
//	store.put(10, std::string(10, 'a'));
//
//	std::cout << "after put ------------" << std::endl;
//	store.show_memtable();
//	std::cout << "-----------------" << std::endl;
//	//store.del(10);
//	//std::cout << "after del------------" << std::endl;
//	//store.show_memtable();
//	//std::cout << "-----------------" << std::endl;
//
//	store.put(10, std::string(1, 's'));
//	std::cout << "after put again------------" << std::endl;
//	store.show_memtable();
//	std::cout << "-----------------" << std::endl;
//	std::cout<<store.get(10)<<std::endl;
//
//
//	std::cout << "finish" << std::endl;
//
//	
//
//	
//	return 0;
//}
