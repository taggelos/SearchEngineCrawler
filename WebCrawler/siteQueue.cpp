//Function Definitions
#include "siteQueue.h"

SiteQueue::Node::Node(char* s){
	next=NULL;
	site = new char[strlen(s)+1];
	strcpy(site,s);
}

SiteQueue::Node::~Node(){
	delete[] site;
}

SiteQueue::SiteQueue(){
	//Initialise our first Node, head
	head = NULL;
	numNodes = 0;
}

void SiteQueue::push(char* site){
	if (head==NULL){
		head = new Node(site);
		numNodes++;
		return;
	}
	//Temp will hold last node
	Node* temp = head;
	while (temp->next!=NULL) {
		//Check if duplicate every node
		if(!strcmp(temp->site,site)) return;
		temp = temp->next;
	}
	//Check last node as well
	if(!strcmp(temp->site,site)) return;
	Node * n = new Node(site);
	temp->next = n;
	numNodes++;
}

char* SiteQueue::pop(){
	if (numNodes==0) return NULL;
	Node* nhead = head->next;
	char* ressite = new char[strlen(head->site)+1];
	strcpy(ressite,head->site);
	delete head;
	numNodes--;
	head=nhead;
	return ressite;
}

int SiteQueue::countNodes(){
	return numNodes;
}

//Destructor
SiteQueue::~SiteQueue(){
	Node* temp = NULL;
	while(head!= NULL){
		temp = head;
		head = head->next;
		delete temp;
	}
}
