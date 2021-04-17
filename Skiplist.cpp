#include "Skiplist.h"

void Skiplist::addHeader()
{
	bool is_empty = header.empty();
	Node* first = new Node;first->type = true;
	Node* last = new Node;last->type = true;
	first->next = last;
	last->prev = first;
	if (!is_empty)
	{
		Node* second = header.front();
		first->down = second;
		second->up = first;
	}
	header.push_front(first);
	return;
}

void Skiplist::removeHeader()
{
	Node* tmp = *header.begin();
	while (tmp)
	{
		Node* cur = tmp;
		tmp = tmp->next;
		delete cur;
	}
}

Skiplist::Skiplist()
{
	srand(time(NULL));
	_size = 0;
}

void Skiplist::init()
{

}

void Skiplist::clear()
{
	for (Node* it : header)
	{
		Node* tmp = it;
		while (tmp)
		{
			Node* cur = tmp;
			tmp = tmp->next;
			delete cur;
		}
		
	}
}

bool Skiplist::get(unsigned long key,std::string& value)
{
	if (header.empty()) return false;
	Node* p = *header.begin(),*enter=*header.begin();
	if (!skipSearch(key, enter, p))
		return false;
	else
		value = p->value;
	return true;	
}

bool Skiplist::skipSearch(unsigned long& key, Node*& enter, Node*& p)
{
	if (!p) return false;
	while (true)
	{
		while (p->next && !p->next->isBoundNode() && (p->next->key <= key))
			p = p->next;
		if (p && (p->key == key))
			return true;
		//if not find in this level
		enter = enter->down;
		if (!enter) return false;//is text book wrong?
		p = (p) ? p->down : enter;
	}
}

/*
return insert Node, p is its pre, b is its down
*/
Node* Skiplist::insertAfterAbove(unsigned long key, std::string value, Node* p, Node* b)
{
	_size++;
	return p->insertAsSuccAbove(key,value,b);

}

Node*& Node::insertAsSuccAbove(unsigned long key, std::string value, Node* b)
{
	Node* insert = new Node(key, value);
	insert->prev = this;
	insert->next = next;
	insert->down = b;
	if (b) b->up = insert;
	next->prev = insert;
	next = insert;
	return insert;
}

void Skiplist::removeNode(Node* p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;
	_size--;
	delete p;
}

bool Skiplist::put(unsigned long key, std::string value)
{
	if (!_size)
	{
		addHeader();
	}
	//find pos to insert,and go to bottom
	Node* p = (*header.begin()),*enter= *header.begin();
	if (skipSearch(key, enter, p))
		while (p->down) p = p->down;

	//p->next is the insert node base
	Node* base = insertAfterAbove(key, value, p, NULL);

	//random decide whether to increase level
	while (rand() % 2)
	{
		//find the tower not lower than this
		while (!p->isBoundNode() && !p->up) p = p->prev;
		//if p is in the header
		if (p->isBoundNode()&&(p == *header.begin()))
		{
			//if already in the top level, add a new level
			addHeader();
		}
		p = p->up;
		//enter = enter->up;//enter is manipulate by next\prev
		base = insertAfterAbove(key,value,p,base);
	}
	return true;
}

bool Skiplist::del(unsigned long key)
{
	if (header.empty()) return false;
	Node* p = *header.begin(),*enter=p;
	if (!skipSearch(key, enter, p))
		return false;
	do {
		Node* lower = p->down;
		removeNode(p);
		p = lower;
	} while (p);
	while (!header.empty() && (header.front()->next == header.back()))
	{
		removeHeader();
	}
	return true;
}

void Skiplist::showSkipList()
{
	for (auto it : header)
	{
		auto tmp = it;
		while (tmp->next&&!tmp->next->isBoundNode())
		{
			tmp = tmp->next;
			std::cout << tmp->key << ' ';
		}
		std::cout << std::endl;
	}
}