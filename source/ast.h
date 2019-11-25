#ifndef _AAST_H
#define _AAST_H
#include <stddef.h>
#include <iostream>
#include <fstream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

class Node
{
  public:
	int Type;
	const char* lexeme;
	Node *son1 = NULL;
	Node *son2 = NULL;
	Node *son3 = NULL;
	Node(const char* lexeme, int type, Node *son1, Node *son2, Node *son3);
};

void toJSON(rapidjson::Document& doc, Node* node);
void tree_print(Node *tree, int n);
#endif
