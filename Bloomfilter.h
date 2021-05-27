#pragma once

#include<stdint.h>
#include<iostream>//test
#include <bitset>//test
#define BUF_SIZE 10240

class Bloomfilter {
	uint8_t bf[BUF_SIZE];

public:
	void add_key(uint64_t key);
	bool not_in_bf(uint64_t key);
	uint8_t* get_bf() { return bf; }
};

