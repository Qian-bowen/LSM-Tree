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
	Node* tmp = header.front();
	header.pop_front();
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
	_pair_size = 0;
	_ubyte = 0;
}

Skiplist::~Skiplist()
{
	clear();
}

void Skiplist::init()
{

}

void Skiplist::clear()
{
	if (header.empty()) return;
	while(!header.empty())
	{
		Node* tmp = header.front();
		header.pop_front();
		while (tmp)
		{
			Node* cur = tmp;
			tmp = tmp->next;
			delete cur;
		}	
	}

	_size = 0;
	_pair_size = 0;
	_ubyte = 0;
}

bool Skiplist::get(uint64_t key,std::string& value)
{
	if (header.empty()) return false;
    Node* p = *header.begin(),*enter=*header.begin();
	if (!skipSearch(key, enter, p))
		return false;
	else
		value = p->value;

	if (value == "~DELETED~")
		return false;
	//std::cout << "get value in memtable:" << value << std::endl;
	return true;	
}

bool Skiplist::skipSearch(uint64_t& key, Node*& enter, Node*& p)
{
	if (!p) return false;
	while (true)
	{
		while (p->next && !p->next->isBoundNode() && (p->next->key <= key))
			p = p->next;
		if (p && !p->isBoundNode()&& (p->key == key))
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
Node* Skiplist::insertAfterAbove(uint64_t key, std::string value, Node* p, Node* b)
{
	_size++;
    Node* insert_node = p->insertAsSuccAbove(key,value,b);
    return insert_node;
}

Node* Node::insertAsSuccAbove(uint64_t key, std::string value, Node* b)
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

bool Skiplist::put(uint64_t key, std::string value)
{
	if (!_size)
	{
		addHeader();
	}
	//find pos to insert,and go to bottom
	//replace police: if keys are identital, just replace it
	Node* p = (*header.begin()),*enter= *header.begin();
	if (skipSearch(key, enter, p))
	{
		int old_ubyte=sizeof(p->key)+p->value.size();
		p->value = value;
		while (p->down)
		{
			p = p->down;
			p->value = value;
		}
		//resize ubyte
		_ubyte=_ubyte-old_ubyte+ sizeof(key) + value.size();
		return true;
	}

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
	++_pair_size;
	_ubyte += sizeof(key) + value.size();
	return true;
}

bool Skiplist::del(uint64_t key)
{
	if (header.empty()) return false;
	Node* p = *header.begin(),*enter=p;
	if (!skipSearch(key, enter, p))
		return false;
	std::string del_val = p->value;
	do {
		Node* lower = p->down;
		removeNode(p);
		p = lower;
	} while (p);
	//while (!header.empty() && (header.front()->next == header.back()))
	while (!header.empty() && (header.front()->next->type == true))
	{
		removeHeader();
	}
	--_pair_size;
	_ubyte =_ubyte - (sizeof(key) + del_val.size());
	return true;
}

void Skiplist::showSkipList()
{
	std::cout << "-----------------" << std::endl;
	for (auto it : header)
	{
		auto tmp = it;
        while (tmp)
		{
			/*std::cout << tmp->key << ' '<<tmp->value<<' ';*/
			std::cout << tmp->key << ' ';
            tmp = tmp->next;
		}
		std::cout << std::endl;
	}
	std::cout << "-----------------" << std::endl;
}
