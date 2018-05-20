//Function Definitions
#include "jobQueue.h"

JobQueue::Node::Node(int f){
	next=NULL;
	fd=f;
}

JobQueue::Node::~Node(){}

JobQueue::JobQueue(){
	//Initialise our first Node, head
	head = NULL;
	numNodes = 0;
}

void JobQueue::push(int fd){
	if (head==NULL){
		head = new Node(fd);
		numNodes++;
		return;
	}
	//Temp will hold last node
	Node* temp = head;
	while (temp->next!=NULL) temp = temp->next;
	Node * n = new Node(fd);
	temp->next = n;
	numNodes++;
}

int JobQueue::pop(){
	if (numNodes==0) return -1;
	Node* nhead = head->next;
	int resfd = head->fd;
	delete head;
	numNodes--;
	head=nhead;
	return resfd;
}

int JobQueue::countNodes(){
	return numNodes;
}

//Destructor
JobQueue::~JobQueue(){
	Node* temp = NULL;
	while(head!= NULL){
		temp = head;
		head = head->next;
		delete temp;
	}
}
