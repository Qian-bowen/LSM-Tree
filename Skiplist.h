#pragma once

#include<string>
#include<list>
#include<vector>
#include<stdlib.h>
#include<ctime>
#include <stdint.h>

#include<iostream>

struct Node {
	uint64_t key;
	std::string value;
	bool type;//true means boundary node
	Node* up, * down, * next, * prev;
    Node():key(0),value(""),type(false),up(NULL),down(NULL),next(NULL),prev(NULL){}
    Node(uint64_t k, std::string v) :key(k),value(v),type(false),up(NULL), down(NULL), next(NULL), prev(NULL) {}

    Node* insertAsSuccAbove(uint64_t key, std::string value, Node* b);
	
	bool isBoundNode() { return type; }
};

class Skiplist {
private:
	std::list<Node*> header;//node are also connect by next
    int _size;//node number in skiplist
	int _pair_size;//key-value size
	int _ubyte;//unique bytes of key-value

	void init();
	

	void addHeader();
	void removeHeader();

	bool skipSearch(uint64_t& key,Node*& enter,Node*& p);
    Node* insertAfterAbove(uint64_t key, std::string value, Node* p, Node* b);
	void removeNode(Node* p);
	

public:
	Skiplist();
	~Skiplist();
	void clear();
	int size()const { return _size; }
	int pair_size()const { return _pair_size; }
	int ubyte_size()const { return _ubyte; }
	bool put(uint64_t key, std::string value);
	bool get(uint64_t key, std::string& value);
	bool del(uint64_t key);

    Node* get_all_header()const
    {
        Node* h=header.back()->next;
        return h;
    }

	void showSkipList();

};

