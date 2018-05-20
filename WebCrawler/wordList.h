// Header --> Functions Declarations
#include <iostream>
#include <cstring> //strcpy
#include <cstdio> //ioctl

using namespace std;

#ifndef WORDLIST_H
#define WORDLIST_H

class WordList {
	struct Node {
		char* word;
		Node* next;
		Node(char* word);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
public:
	WordList();
	void add(char* word);
	int countWords();
	char** returnAsArray();
	~WordList();
};

#endif
