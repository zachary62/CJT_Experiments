#pragma once
#include <relation.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "operator.h"
#include "simplex.h"

class JunctionTree
{
public:

    JunctionTree(vector<string> allAttrs);

    Relation* readRelation(string filePath, string name);
    Relation* readEmptyRelation(vector<string> attrs, string name);

    void addEdge(string name1, string name2);
    bool checkAcyclic();
    bool acycdfs(string cur, string parent, unordered_set<string>& seen);
    void pull(string cur, string parent);
    void push(string cur, string parent);
    void pushdelete(string cur, string parent);
    void sendMessage(string from, string to);
    bool hasRelation(string name);
    bool hasMessage(string from, string to);
    void deleteMessage(string from, string to);
    Relation* absorb(string name, unordered_set<string> kept_attr);

    ~JunctionTree();

    Operator op;
    vector<string> allAttrs;
    bool what_if = false;
    unordered_map<string, Relation*> relations;
    unordered_map<string, Relation*> what_if_relations;
    unordered_map<string, unordered_set<string>> edges;
    unordered_map<string, Relation*> message;
    unordered_map<string, Relation*> what_if_message;
    

};

