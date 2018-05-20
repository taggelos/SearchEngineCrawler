//Function Definitions
#include "wordList.h"
using namespace std;

//Store the word into a Node
WordList::Node::Node(char* w){
	next = NULL;
	word = new char[strlen(w)+1];
	strcpy(word,w);
}

//Delete the word
WordList::Node::~Node(){
	delete[] word;
}

WordList::WordList(){
	//Initialise head with NULL
	head = NULL;
	numNodes = 1;
}

//Insertion of input taken into the List
void WordList::add(char* word){
	// O ( 1 ) insertion
	if (head==NULL){
		head = new Node(word);
		return;
	}
	Node * n = new Node(word);
	n->next = head;
	head = n;
	numNodes++;
}

char** WordList::returnAsArray(){
	char** words = new char*[numNodes];
	Node* temp = head;
	int i=0;
	while(temp!= NULL){
		words[i] = new char[strlen(temp->word)+1];
		strcpy(words[i++],temp->word);
		temp = temp->next;
	}
	return words;
}

int WordList::countWords(){
	return numNodes;
}

//WordList Destructor
WordList::~WordList(){
	Node* temp = NULL;
	while(head!= NULL){
		temp = head;
		head = head->next;
		delete temp;
	}
}
