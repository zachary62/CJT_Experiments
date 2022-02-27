#pragma once
#include "relation.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;



class Operator
{
public:
    Operator();
    ~Operator();
    hNode* buildHashNode(Relation* input, vector<int> idx);
    Relation* marginalize(Relation* R, unordered_set<string> attrs);
    Relation* join(vector<Relation*> const& relations, vector<string> const& allAttrs);
    Relation* joinAndMarginalize(vector<Relation*> const& relations, vector<string> const& allAttrs, unordered_set<string> const& keptAttrs);
    Relation* generializedProject(Relation* input, vector<string> attrs);
    void trieToRelation(vector<row>* relation, vector<string> curRow, hNode* curNode);
};

