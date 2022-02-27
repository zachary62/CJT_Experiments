#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class trieNode{
  public:
    trieNode() = default;
    virtual ~trieNode() = default;
};

struct row
{
  vector<int> attr;
  // aggregation value
  int value;

  void print(){
    for(int s: attr){
        cout << s <<",";
    }
    cout << value;
    cout << "\n";
  };
};

class trieNodeValue: public trieNode{
  public:
    int value;
    void print();
};


class Trie;

class trieNodeInner: public trieNode{
  public:
    unordered_map<int,trieNode*> children;
    trieNodeInner* insertNode(int att);
    bool insertValue(int att, int value=0);
    bool insertNodes(row const& r);
    void print(int total_level);
    void addTo(int total_level, Trie & destTrie);
    Trie* marginalize(vector<bool> const& keptAttrs, int total_level);
    
    Trie* sample(int rate, int total_level);
    void indicator(int total_level);
    void domain(int total_level, unordered_map<string, unordered_set<int>> & values, vector<string> const& schema);

    
  private:
    void addToNext(row* r, int total_level, int cur_level, Trie & destTrie);
    void print_next(row* r, int total_level, int cur_level);
    void marginalize_next(vector<bool> const& keptAttrs, row* r, Trie* trie, int total_level, int cur_level );
    void indicator_next(int total_level, int cur_level);
    void sample_next(row* r, Trie* trie, int total_level, int cur_level, int rate);
    void domain_next(row* r, int total_level, int cur_level, unordered_map<string, unordered_set<int>> & values, vector<string> const& schema);
};

class Trie
{
  public:
    Trie(int level);
    trieNode* trieroot;
    int level;
    int size;
    int f2;
    void insertNodes(row const& r);
    void print();
    void indicator();
    void addTo(Trie & destTrie);
    Trie* sample(int rate);
    Trie* marginalize(vector<bool> const& keptAttrs);
    void domain(unordered_map<string, unordered_set<int>> & values, vector<string> const& schema);
};
