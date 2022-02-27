#pragma once
#include <trie.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// multi-level hash map
struct hNode{
    unordered_map<int,hNode*> children;
    // assume that leaf node has non-zero value
    int value = 0;
};





class Relation
{
public:
    Relation();
    Relation(std::string dir, vector<string> allAttrs);
    // empty relation with attributes only
    Relation(vector<string> attrs, vector<string> allAttrs, bool empty);
    ~Relation();
    void print();
    Relation* marginalize(vector<bool> const& keptAttrs);
    Relation* marginalize(unordered_set<string> const& margAttrs);
    Relation* marginalizeKept(unordered_set<string> const& keptAttrs);
    void domain(unordered_map<string, unordered_set<int>> & values);
    void add(Relation* other);
    bool empty();

    void indicator();
    Relation* sample(int rate);
    int size(){
      if (empty()){
        return 0;
      }
      return trie->size;
    };

    float marg_size(float rate){
//         cout << rate << " " << trie->size << " " << trie->f2  <<"\n";
      return rate * (float) (max(trie->size - trie->f2,0)) + (float)trie->f2;
//         return sqrt(rate) * (float) (max(trie->size - trie->f2,0)) + (float)trie->f2;
    };
    

    // original attributes from the file
    vector<string> schema;

    // hNode* trie;
    Trie* trie = nullptr;

    int id = -1;
};

