// Header --> Functions Declarations
#include <iostream>
#include <cstring>
using namespace std;

#ifndef SITEQUEUE_H
#define SITEQUEUE_H

//Our Queue
class SiteQueue {
	struct Node {
		//File descriptor for specific socket
		char* site;
		Node* next;
		Node(char* site);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
public:
	SiteQueue();
	void push(char* site);
	char* pop();
	int countNodes();
	bool exists(char* site);
	~SiteQueue();	
};

#endif
