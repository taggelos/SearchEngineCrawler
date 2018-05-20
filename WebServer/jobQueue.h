// Header --> Functions Declarations
#include <iostream>
using namespace std;

#ifndef JOBQUEUE_H
#define JOBQUEUE_H

//Our Queue
class JobQueue {
	struct Node {
		//File descriptor for specific socket
		int fd;
		Node* next;
		Node(int fd);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
public:
	JobQueue();
	void push(int fd);
	int pop();
	int countNodes();
	~JobQueue();	
};

#endif
