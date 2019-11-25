#ifndef _AAST_H
#define _AAST_H

#include <stddef.h>
#include <iostream>
#include <string>
#include <fstream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

class PNode
{
  public:
	int Type;
	const char* lexeme;
	PNode *son1;
	PNode *son2;
	PNode *son3;
	PNode();
};

class Node : public PNode
{
  public:
	Node(const char* lexeme, int type, PNode *son1, PNode *son2, PNode *son3);

};

class StringNode : public PNode
{
  public:
	StringNode(const char* lexeme, int type);
};

class NumberNode : public PNode
{
  public:
	NumberNode(const char* lexeme, int type);
};

class VariableNode : public PNode
{
  public:
	VariableNode(const char* lexeme, int type);
};

class BinExprNode : public PNode
{
  public:
	BinExprNode(const char* lexeme, int type, PNode *son1, PNode *son2, PNode *son3);
};

void toJSON(rapidjson::Document& doc, PNode* node);
void tree_print(PNode *tree, int n);
#endif
