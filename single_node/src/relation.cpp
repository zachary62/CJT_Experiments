#include "relation.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>

// based on https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c

static const char COMMENT_CHAR = '#';
static const char PARAMETER_SEPARATOR_CHAR = ',';

static const bool VERBOSE = true;

int i = 0;

using namespace std;

Relation::Relation()
{ 
    // relation = new vector<row>;
    // trie = new Trie();
    id = i++;
    // cout<<id<<"\n";
}

enum class CSVState {
    UnquotedField,
    QuotedField,
    QuotedQuote
};

bool Relation::empty(){
    if(trie)
        return false;
    return true;
}

// read an empty relation
Relation::Relation(vector<string> attrs, vector<string> allAttrs, bool empty)
{ 
    for(string attr: allAttrs){
    if(find(attrs.begin(), attrs.end(), attr) != attrs.end()){
            schema.push_back(attr);
        }
    }

    if(schema.size() != attrs.size()){
        cout << "Relation contains extra attributes\n";
        exit(1);
    }

    if(!empty){
        trie = new Trie(schema.size());
    }

}


Relation::Relation(string dir, vector<string> allAttrs)
{   
    id = i++;
    // cout<<id<<"\n";
    

    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "table" <<" does not exist. \n";
		exit(1);
	}

    // get the schema
    getline(input, line);
    ssLine << line;

    string temp;    

    vector<string> attrs;
    // read each attribute name
    while(getline(ssLine, temp, PARAMETER_SEPARATOR_CHAR)){
        if(find(attrs.begin(), attrs.end(), temp) != attrs.end()){
            cout << "Relation " << dir << " contains duplicated attribute " << temp << "\n";
            exit(1);
        }
        attrs.push_back(temp);
    }

    for(string attr: allAttrs){
        if(find(attrs.begin(), attrs.end(), attr) != attrs.end()){
            schema.push_back(attr);
        }
    }

    if(schema.size() != attrs.size()){
        cout << "Relation " << dir << " contains extra attributes\n";
        exit(1);
    }

    // index between attrs and schema
    // for instance, if attrs = [A,B,C], schema = [C,A,B]
    // idx = [1,2,0]
    vector<unsigned int> idx;

    for(string attr: attrs){
        for(unsigned int i = 0; i < schema.size(); i++){
            if(schema[i] == attr){
                idx.push_back(i);
                break;
            }
        }
    }

    row r;
    r.value = 1;
    for(unsigned int i = 0; i < schema.size(); i++){
        r.attr.push_back(0);
    }

    ssLine.clear();

    trie = new Trie(schema.size());

    while(getline(input, line)){

        // cout << line <<"\n";
        if (line[0] == COMMENT_CHAR || line == "")
                continue;
        
        // row r = readCSVRow(line);


        ssLine << line;
        
        int i = 0;
        // read each attribute value
        while(getline(ssLine, temp, PARAMETER_SEPARATOR_CHAR)){

            r.attr[idx[i]] = stoi(temp);
            i++;
            // cout<< stoi(temp) << "\n";
        }

        if (r.attr.size() != schema.size())
        {
            cout<< dir << " Different number of attributes. \n";
            // cout<< r.attr.size() << "\n";
            // cout<< schema.size() << "\n";
            exit(1);
        }
        
        trie->insertNodes(r);
        ssLine.clear();
    }
}


// release the memory after join
void deleteNode(hNode* curNode){
    // base case
    if(curNode->value != 0){
        // the node is no longer used
        delete curNode;
        return;
    }

    // iterate through the hashmap in the node
    unordered_map<int,hNode*>::iterator it;
    for (it = curNode->children.begin(); it != curNode->children.end(); it++)
    {
        deleteNode(it->second);
    }
    // the node is no longer used
    delete curNode;
}

Relation::~Relation()
{   
    // for(string* s: *schema){
    //     delete s;
    // }

    // for(row* r: *relation){
    //     for(string* s: *r->attr){
    //         delete s;
    //     }
    //     delete r;
    // }

    // deleteNode(trie);
}

void Relation::print()
{   
    for(string attr: schema){
        cout<< attr << ", ";
    }
    cout << "\n";
    trie->print();
}

void Relation::indicator()
{   
    trie->indicator();
}

Relation* Relation::sample(int rate)
{   
    Relation* relation = new Relation();
    relation->trie = trie->sample(rate);
    relation->schema = schema;
    return relation;
}

Relation* Relation::marginalize(vector<bool> const& keptAttrs){
    Relation* relation = new Relation();
    relation->trie = trie->marginalize(keptAttrs);
    for(unsigned int i = 0; i < keptAttrs.size(); i++){
        if(keptAttrs[i]){
            relation->schema.push_back(schema[i]);
        }
    }
    return relation;
}

void Relation::domain(unordered_map<string, unordered_set<int>> & values){
    this->trie->domain(values, schema);
}


Relation* Relation::marginalize(unordered_set<string> const& margAttrs){
    vector<bool> keptAttrs;
    unsigned int count = 0;
    for(string att: schema){
        if(margAttrs.find(att) == margAttrs.end()){
            keptAttrs.push_back(true);
        }
        else{
            keptAttrs.push_back(false);
            count++;
        }
    }
    if(count != margAttrs.size()){
        cout << "Unexpected attributes for marginalization!\n";
        cout << "relation schema: ";
        for(string att: schema){
            cout << att << ", ";
        }
        cout << "\n";
        cout << "marg att: ";
        for(string att: margAttrs){
            cout << att << ", ";
        }
        cout << "\n";

        exit(1);
    }
    return marginalize(keptAttrs);
}

Relation* Relation::marginalizeKept(unordered_set<string> const& keptAttrs){
    vector<bool> keptAttrsBool;
    unsigned int count = 0;
    for(string att: schema){
        if(keptAttrs.find(att) != keptAttrs.end()){
            keptAttrsBool.push_back(true);
            count++;
        }
        else{
            keptAttrsBool.push_back(false);
        }
    }
    if(count != keptAttrs.size()){
        cout << "Unexpected attributes for marginalization!\n";
        exit(1);
    }
    return marginalize(keptAttrsBool);
}

void Relation::add(Relation* other){
    other->trie->addTo(*trie);
}