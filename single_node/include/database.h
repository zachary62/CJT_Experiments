#pragma once
#include <relation.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "operator.h"
#include "simplex.h"

class Database
{
public:
    // Database(std::string dir);
    // Database(vector<Relation*> relations);
    Database(vector<string> allAttrs, int sample_rate = 0);

    Relation* readRelation(string filePath);

    // Order by Degree as recommended in EmptyHeaded
    void GetNodeOrder();
    // Order the given attrs based on node order
    vector<string> orderAttrs(vector<string> attrs);

    void buildTrie(Relation* relation);

    // get the basic stats and build trie for each relation,
    void Preprocess();
    void updateEdges();
    void computeAllStats();
    void computeStats(string attribute);
    vector<Relation*> getIndicator(vector<int> const& idx, unordered_set<string> const& attrs);

    long long int computeDegree(string attribute);
    void minDegree();

    long long int computeFill(string attribute);
    void minFill();

    double computeAGMjoin(string attribute);
    void minAGMjoin();

    double computeAGMmarg(string attribute);
    void minAGMmarg();

    Relation* getSample(Relation* relation);

    long long int computeSamplejoin(string attribute);
    void minSamplejoin();

    long long int computeSamplemarg(string attribute);
    void minSamplemarg();

    vector<int> findRelatedRelations(string attribute);

    // join and build trie for the join result
    Relation* join(vector<Relation*> relations);

    // join and build trie for the join result
    Relation* marginalize(Relation* relation, string attr);

    // Relation* project(Relation* relation, vector<string> newAttrs);

    ~Database();

    void eliminate(string attribute);

    int sample_rate;
    Operator op;
    vector<string> allAttrs;
    vector<Relation*> relations;
    unordered_map<string, Relation*> cache;
    unordered_map<string, vector<long long int>> join_sample_rate;
    unordered_map<string, long long int> sample_result_cache;
    unordered_map<string, int> domainSize;
    unordered_map<string, unordered_set<string>> edges;
    double sampleduration = 0;
    double indicduration = 0;
    bool isindicatorprojection = true;


};

