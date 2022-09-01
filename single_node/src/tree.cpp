#include "tree.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <fstream>
#include <math.h>  
#include <sstream>
#include <algorithm>
#include "operator.h"
#include <climits>

using namespace std;

JunctionTree::JunctionTree(vector<string> allAttrs)
{  
    this->allAttrs = allAttrs;
}


JunctionTree::~JunctionTree()
{   

}

Relation* JunctionTree::readRelation(string filePath, string name){

//     if(this->relations.find(name) != this->relations.end()){
//         cout << "Relation " << name << " has already been read!\n";
//         exit(0);
//     }

    Relation* r = new Relation(filePath, this->allAttrs);

    this->relations[name] = r;

    return r;
}

Relation* JunctionTree::readEmptyRelation(vector<string> attrs, string name){
    if(this->relations.find(name) != this->relations.end()){
        cout << "Relation " << name << " has already been read!\n";
        exit(0);
    }

    Relation* r = new Relation(attrs, this->allAttrs, true);

    this->relations[name] = r;

    return r;
}

void JunctionTree::addEdge(string name1, string name2){
    if(this->relations.find(name1) == this->relations.end()){
        cout << "Relation " << name1 << " can't be found!\n";
        exit(0);
    }

    if(this->relations.find(name2) == this->relations.end()){
        cout << "Relation " << name2 << " can't be found!\n";
        exit(0);
    }

    this->edges[name1].insert(name2);
    this->edges[name2].insert(name1);
}

bool JunctionTree::checkAcyclic(){
    if(this->relations.size() == 0){
        cout << "Junction tree is empty!\n";
        return false;
    }

    unordered_set<string> seen;
    
    // get one relation and dfs
    for(auto kv : this->relations) {
        if(!acycdfs(kv.first, "", seen)){
            cout << "Junction tree is cyclic!\n";
            return false;
        }
        break;
    }

    if(seen.size() != this->relations.size()){
        cout << "Inconsistent size for seen and relations\n";
        cout << "Seen: ";
        for(string r: seen){
            cout << r << ", ";
        }
        cout << "\nRelations: ";
        for(auto kv: this->relations){
            cout << kv.first << ", ";
        }
        cout << "\n";
        return false;
    }
    return true;

}

bool JunctionTree::hasRelation(string name){
    return this->relations.find(name) != this->relations.end();
}

bool JunctionTree::hasMessage(string from, string to){
    return this->message.find(from + "-" + to) != this->relations.end();
}
void JunctionTree::deleteMessage(string from, string to){
    this->message.erase(from + "-" + to);
}

bool JunctionTree::acycdfs(string cur, string parent, unordered_set<string>& seen){
    seen.insert(cur);
    for(string neighbour: this->edges[cur]){
        if(parent != "" && neighbour == parent){
            continue;
        }
        if(seen.find(neighbour) != seen.end()){
            return false;
        }
        if(!acycdfs(neighbour, cur, seen)){
            return false;
        }
    }
    return true;
}


void JunctionTree::pull(string cur, string parent){
    for(string child: this->edges[cur]){
        if(parent != "" && child == parent){
            continue;
        }
        if(!hasMessage(child, cur))
            pull(child, cur);
    }
    if(parent != ""){
        sendMessage(cur, parent);
    }
}

void JunctionTree::push(string cur, string parent){
    for(string child: this->edges[cur]){
        if(parent != "" && child == parent){
            continue;
        }
        sendMessage(cur, child);
        push(child, cur);
    }
}

void JunctionTree::pushdelete(string cur, string parent){
    for(string child: this->edges[cur]){
        if(parent != "" && child == parent){
            continue;
        }
        deleteMessage(cur, child);
        pushdelete(child, cur);
    }
}


unordered_set<string> schema_intersect(vector<string> const& schema1, vector<string> const& schema2 ){
    unordered_set<string> schema2_set(schema2.begin(), schema2.end());
    unordered_set<string> result;
    for(string att: schema1){
        if(schema2_set.find(att) != schema2_set.end()){
            result.insert(att);
        }
    }
    return result;
}

void JunctionTree::sendMessage(string from, string to){
    if(hasMessage(from, to))
        return;

    cout << "Sending message from " << from << " to " << to << "\n";

    vector<Relation*> to_join_relations;
    
    if(!hasRelation(from)){
        cout << "Relation " << from << " can't be found!\n";
        exit(0);
    }

    if(!relations[from]->empty()){
        to_join_relations.push_back(relations[from]);
    }

    

    for(string child: this->edges[from]){
        if(to != "" && child == to){
            continue;
        }
        // check if relaiton exist!!
        if(!hasMessage(child, from)){
            cout << "Message " << child << "-" << from << " can't be found!\n";
            exit(0);
        }
        to_join_relations.push_back(message[child + "-" + from]);

    }

    unordered_set<string> intersec = schema_intersect(relations[from]->schema, relations[to]->schema);
    
    cout << "Schema: ";
    for(string s: intersec){
        cout << s << ", ";
    }
    cout << "\n";
    cout << "Join relation number: " << to_join_relations.size() << "\n";

    Relation* cur_message = op.joinAndMarginalize(to_join_relations, allAttrs, intersec);
    cout << "Join result Size: " << cur_message->size() << "\n";
    message[from + "-" + to] = cur_message;

}

Relation* JunctionTree::absorb(string name, unordered_set<string> kept_attr){
    pull(name, "");
    if(this->relations.find(name) == this->relations.end()){
        cout << "Relation " << name << " can't be found!\n";
        exit(0);
    }

    vector<Relation*> to_join_relations;
    if(!relations[name]->empty()){
        to_join_relations.push_back(relations[name]);
    }


    for(string child: this->edges[name]){
        to_join_relations.push_back(message[child + "-" + name]);
    }

    cout << "Absorb size: " << to_join_relations.size() << "\n";

    return op.joinAndMarginalize(to_join_relations, allAttrs, kept_attr);
}
