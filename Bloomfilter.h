#pragma once

#include<stdint.h>

class Bloomfilter {
	uint8_t bf[10240];

public:
	void add_key(uint64_t key);
	bool not_in_bf(uint64_t key);
	uint8_t* get_bf() { return bf; }
};

