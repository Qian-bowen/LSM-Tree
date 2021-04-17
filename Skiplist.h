#pragma once
#include<string>
#include<list>
#include<stdlib.h>
#include<ctime>

#include<iostream>

struct Node {
	unsigned long key;
	std::string value;
	bool type;//true means boundary node
	Node* up, * down, * next, * prev;
	Node():up(NULL),down(NULL),next(NULL),prev(NULL),type(false){}
	Node(unsigned long k, std::string v) :key(k),value(v),up(NULL), down(NULL), next(NULL), prev(NULL),type(false) {}

	Node*& insertAsSuccAbove(unsigned long key, std::string value, Node* b);
	
	bool isBoundNode() { return type; }
};

class Skiplist {
private:
	std::list<Node*> header;//node are also connect by next
	int _size;

	void init();
	void clear();

	void addHeader();
	void removeHeader();

	bool skipSearch(unsigned long& key,Node*& enter,Node*& p);
	Node* insertAfterAbove(unsigned long key, std::string value, Node* p, Node* b);
	void removeNode(Node* p);
	

public:
	Skiplist();
	int size()const { return _size; };
	bool put(unsigned long key, std::string value);
	bool get(unsigned long key, std::string& value);
	bool del(unsigned long key);

	void showSkipList();

};

