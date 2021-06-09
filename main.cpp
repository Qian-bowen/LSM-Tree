//#include<iostream>
//#include "kvstore_api.h"
//#include "kvstore.h"
//
//
//
//int main()
//{
//	MemCache store("store");
//	store.reset();
//	int max = 6000;
//	int i;
//
//	for (i = 0; i < max*5; ++i) {
//		store.put(i, std::string(i + 1, 's'));
//	}
//	store.dump2sst();
//	for (i = max*5; i < max*10; ++i) {
//		store.put(i, std::string(i + 1, 's'));
//	}
//	store.dump2sst();
//
//
//	for (i = 0; i < max*10; ++i) {
//		store.del(i);
//	}
//	return 0;
//}
