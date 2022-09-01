#include "trie.h"

// insert an attribute to trie node
trieNodeInner* trieNodeInner::insertNode(int att){

    // check if string is in the children
    if(children.find(att) == children.end()){
        // if not, add the hash node
        children[att] = new trieNodeInner();
    }

    // move to the child node
    return dynamic_cast<trieNodeInner*>(children[att]);
    
}

// insert a value node to trie node
bool trieNodeInner::insertValue(int att, int value){

    bool isNew = false;
    // check if string is in the children
    // "nodevalue" is the key for value node
    if(children.find(att) == children.end()){
        // if not, add the hash node
        children[att] = new trieNodeValue();
        isNew = true;
    }
    // move to the child node

    trieNodeValue* node = dynamic_cast<trieNodeValue*>((children[att]));
    node->value += value;
    return isNew;
    
}

// insert a row to trie node
bool trieNodeInner::insertNodes(row const& r){
    trieNodeInner* cur = this;

    for(unsigned int i = 0; i < r.attr.size() -1 ; i++){
        int att = r.attr[i];
        cur = cur->insertNode(att);
    }

    return cur->insertValue( r.attr.back(), r.value);
}

void trieNodeInner::print_next(row* r, int total_level, int cur_level){

    unordered_map<int, trieNode*>::iterator it = this->children.begin();

    // if last level
    if(total_level == cur_level + 1){
        
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            r->attr[cur_level] = it->first;
            r->value = dynamic_cast<trieNodeValue*>(it->second)->value;
//             r->print();
            it++;
        }
    }
    else{
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            
            r->attr[cur_level] = it->first;
            dynamic_cast<trieNodeInner*>(it->second)->print_next(r, total_level, cur_level+1);
            it++;
        }
    }
}

void trieNodeValue::print(){
    cout << value <<"\n";
}


void trieNodeInner::print(int total_level){
    row* r = new row();
    for(int i = 0; i < total_level; i++){
        r->attr.push_back(0);
    }
    print_next(r, total_level, 0);
}



void trieNodeInner::addToNext(row* r, int total_level, int cur_level, Trie & destTrie){

    unordered_map<int, trieNode*>::iterator it = this->children.begin();

    // if last level
    if(total_level == cur_level + 1){
        
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            r->attr[cur_level] = it->first;
            r->value = dynamic_cast<trieNodeValue*>(it->second)->value;
            destTrie.insertNodes(*r);
            it++;
        }
    }
    else{
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            
            r->attr[cur_level] = it->first;
            dynamic_cast<trieNodeInner*>(it->second)->addToNext(r, total_level, cur_level+1, destTrie);
            it++;
        }
    }
}


void trieNodeInner::addTo(int total_level, Trie & destTrie){
    row* r = new row();
    for(int i = 0; i < total_level; i++){
        r->attr.push_back(0);
    }
    addToNext(r, total_level, 0, destTrie);
}


void Trie::addTo(Trie & destTrie){
    if(level == 0)
        cout << "add to 0 level trie!!\n";
        // dynamic_cast<trieNodeValue*>(trieroot)->print();
    else
        dynamic_cast<trieNodeInner*>(trieroot)->addTo(level, destTrie);
}

Trie::Trie(int level){
    if(level == 0)
        trieroot = new trieNodeValue();
    else
        trieroot = new trieNodeInner();
    size = 0;
    f2 = 0;
    this->level = level;
}

void Trie::insertNodes(row const& r){
    if ((int)r.attr.size() != level){
        cout << "Insert row with " << r.attr.size() << " attributes, but level is " << level <<"\n";
        exit(1);
    }

    if(level == 0){
        dynamic_cast<trieNodeValue*>(trieroot)->value += r.value;
    }
    else if(dynamic_cast<trieNodeInner*>(trieroot)->insertNodes(r)){
        size++;
    }else{
        f2++;
    };
}

void Trie::print(){
    if(level == 0)
        dynamic_cast<trieNodeValue*>(trieroot)->print();
    else
        dynamic_cast<trieNodeInner*>(trieroot)->print(level);
}

Trie* Trie::sample(int rate){

    if(level == 0)
        return this;
    else
        return dynamic_cast<trieNodeInner*>(trieroot)->sample(rate, level);
}

Trie* trieNodeInner::marginalize(vector<bool> const& keptAttrs, int total_level){
    int new_level = 0;
    for (bool kept : keptAttrs){
        if(kept)
            new_level ++;
    }
    Trie* trie = new Trie(new_level);
    row* r = new row();
    marginalize_next(keptAttrs, r, trie, total_level,0);
    return trie;
}

void trieNodeInner::domain(int total_level, unordered_map<string, unordered_set<int>> & values, vector<string> const& schema){
    Trie* trie = new Trie(total_level);
    row* r = new row();
    for(int i = 0; i < total_level; i++){
        r->attr.push_back(0);
    }
    domain_next(r,total_level, 0, values, schema);
}

void trieNodeInner::domain_next(row* r, int total_level, int cur_level, unordered_map<string, unordered_set<int>> & values, vector<string> const& schema){
    unordered_map<int, trieNode*>::iterator it = this->children.begin();

    // if last level
    if(total_level == cur_level + 1){
        
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            r->attr[cur_level] = it->first;
            r->value = dynamic_cast<trieNodeValue*>(it->second)->value;

            for(unsigned int i = 0; i < schema.size(); i++){
                values[schema[i]].insert(r->attr[i]);
            }

            it++;
        }
    }
    else{
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            
            r->attr[cur_level] = it->first;
            dynamic_cast<trieNodeInner*>(it->second)->domain_next(r, total_level, cur_level+1, values, schema);
            it++;
        }
    }
}

Trie* trieNodeInner::sample(int rate, int total_level){
    Trie* trie = new Trie(total_level);
    row* r = new row();
    for(int i = 0; i < total_level; i++){
        r->attr.push_back(0);
    }
    sample_next(r, trie, total_level, 0, rate);
    // at least sample one 
    if(trie->size == 0){
        trie->insertNodes(*r);
    }
    return trie;
}

void trieNodeInner::sample_next(row* r, Trie* trie, int total_level, int cur_level, int rate){
    unordered_map<int, trieNode*>::iterator it = this->children.begin();

    // if last level
    if(total_level == cur_level + 1){
        
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            r->attr[cur_level] = it->first;
            r->value = dynamic_cast<trieNodeValue*>(it->second)->value;
            
            if(rand() % rate == 0){
                trie->insertNodes(*r);
            }

            it++;
        }
    }
    else{
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            
            r->attr[cur_level] = it->first;
            dynamic_cast<trieNodeInner*>(it->second)->sample_next(r,trie, total_level, cur_level+1,rate);
            it++;
        }
    }
}

void trieNodeInner::indicator_next( int total_level, int cur_level){

    unordered_map<int, trieNode*>::iterator it = this->children.begin();

    // if last level
    if(total_level == cur_level + 1){
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            dynamic_cast<trieNodeValue*>(it->second)->value = 1;
            it++;
        }
    }
    else{
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            dynamic_cast<trieNodeInner*>(it->second)->indicator_next(total_level, cur_level+1);
            it++;
        }
    }
}


void trieNodeInner::indicator(int total_level){
    indicator_next(total_level, 0);
}

void Trie::indicator(){
    if(level == 0)
        dynamic_cast<trieNodeValue*>(trieroot)->value = 1;
    else
        dynamic_cast<trieNodeInner*>(trieroot)->indicator(level);
}

void trieNodeInner::marginalize_next(vector<bool> const& keptAttrs, row* r, 
                                    Trie* trie, int total_level, int cur_level ){
    unordered_map<int, trieNode*>::iterator it = this->children.begin();
    // if last level
    if(total_level == cur_level + 1){
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            if(keptAttrs[cur_level])
                r->attr.push_back(it->first);
            r->value = dynamic_cast<trieNodeValue*>(it->second)->value;
            trie->insertNodes(*r);
            if(keptAttrs[cur_level])
                r->attr.pop_back();
            it++;
        }
    }
    else{
        // Iterate over the map using iterator
        while(it != this->children.end())
        {
            if(keptAttrs[cur_level])
                r->attr.push_back(it->first);
            dynamic_cast<trieNodeInner*>(it->second)->marginalize_next(keptAttrs, r, trie, total_level, cur_level+1);
            if(keptAttrs[cur_level])
                r->attr.pop_back();
            it++;
        }
    }
}


Trie* Trie::marginalize(vector<bool> const& keptAttrs){
    if((int)keptAttrs.size() != level){
        cout<< "Inconsistent size for marginalization. Current trie level: " << level <<". Received vector size: " << keptAttrs.size() <<"\n";
        exit(1);
    }

    if(level == 0)
        return this;
    else
        return dynamic_cast<trieNodeInner*>(trieroot)->marginalize(keptAttrs, level);
}

void Trie::domain(unordered_map<string, unordered_set<int>> & values, vector<string> const& schema){
    if(level != 0)
        dynamic_cast<trieNodeInner*>(trieroot)->domain(level, values, schema);
}