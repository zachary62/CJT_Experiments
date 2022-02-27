#include "database.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <fstream>
#include <math.h>  
#include <sstream>
#include <algorithm>
#include "operator.h"
#include <climits>

using namespace std;



Database::Database(vector<string> allAttrs, int sample_rate)
{  
    this->allAttrs = allAttrs;
    this->sample_rate = sample_rate;
}

// Database::Database(vector<Relation*> relations)
// {  
//     this->relations = relations;
// }

Database::~Database()
{   

}

Relation* Database::readRelation(string filePath){
    Relation* r = new Relation(filePath, this->allAttrs);
    this->relations.push_back(r);

    if(sample_rate != 0){
        getSample(r);
    }

    return r;
}

Relation* Database::getSample(Relation* relation){
    string hash = "";
    hash.append(to_string(relation->id));
    hash.append(":");
    hash.append(":sample");
    if(cache.find(hash) == cache.end()){
        Relation* sample = relation->sample(sample_rate);
        cache[hash] = sample;
    }
    return cache[hash];
}

// bool sortByDg(const pair<string, int> &a, 
//                const pair<string, int> &b) 
// { 
//     return (a.second > b.second); 
// } 

// void Database::GetNodeOrder(){

//     // get all attributes without duplication
//     // join attribute first
//     map<string,int> attributes;
//     for(Relation* relation: relations){
//         for(string att: relation->schema){
//             if (attributes.find(att) == attributes.end()) {
//                 attributes[att] = 1;
//             }
//             else{
//                 attributes[att] ++;
//             }
//         }
//     }

//         // sort the attribute by degree

//     // create a empty vector of pairs
// 	vector<pair<string, int>> vec;

//     // copy key-value pairs from the map to the vector
//     map<string, int> :: iterator it2;
//     for (it2=attributes.begin(); it2!=attributes.end(); it2++) 
//     {
//         vec.push_back(make_pair(it2->first, it2->second));
//     }

//     // // sort the vector by increasing order of its pair's second value
//     sort(vec.begin(), vec.end(), sortByDg); 

//     for (auto& p: vec){
//         nodeOrder.push_back(p.first);
//         // cout << p.first <<"\n";
//     }
// }



// vector<string> Database::orderAttrs(vector<string> attrs){
//     unordered_set<string> attrs_set;

//     for(string attr: attrs){
//         attrs_set.insert(attr);
//     }

//     vector<string> result;

//     for(string attr: nodeOrder){
//         if(attrs_set.find(attr) != attrs_set.end()){
//             result.push_back(attr);
//         }
//     }

//     return result;
// }



// void Database::buildTrie(Relation* relation){
//     relation->orderedAttr = orderAttrs(relation->schema);
//     relation->buildIdx();
    
//     Operator op;
//     relation->trie = op.buildHashNode(relation, relation->idx);
// }



Relation*  Database::join(vector<Relation*> relations){
    Relation* joinResult = op.join(relations, allAttrs);
    return joinResult;
}

Relation*  Database::marginalize(Relation* relation, string attr){
    unordered_set<string> margAttr = {attr};
    Relation* margResult = relation->marginalize(margAttr);
    return margResult;
}

// // Relation*  Database::project(Relation* relation, vector<string> newAttrs){
// //     Operator op;
// //     Relation* margResult = op.generializedProject(relation, newAttrs);

// //     vector<int> empty;
// //     // build relation from the built multi-level map
// //     op.trieToRelation(margResult->relation, empty, margResult->trie);

// //     margResult->schema = newAttrs;
// //     margResult->buildIdx();
// //     return margResult;
// // }

// return a list of attribute intersection
// because b is sorted, the result is also sorted
vector<string> intersection(unordered_set<string> a, vector<string> b){
    vector<string> result;
    for(string s : b){
        if(a.find(s) != a.end()){
            result.push_back(s);
        }
    }
    return result;

}

// given attribute, return the indices of all relations with this attribute in ascending order
vector<int> Database::findRelatedRelations(string attribute){
    // indices of relations 
    vector<int> idx;

    for(unsigned int i = 0; i < this->relations.size(); i++){
        
        Relation* relation = this->relations[i];
        
        // check if this relation contains attribute
        if(find(relation->schema.begin(), relation->schema.end(), attribute) != relation->schema.end()){
            // if contains
            idx.push_back(i);
        }
    }
    return idx;
}

void Database::computeAllStats(){
    // compute domain size
    unordered_map<string, unordered_set<int>> values;

    for(Relation* r: relations){
        r->domain(values);
    }

    for(string att: allAttrs){
        if(values.find(att) == values.end()){
            cout << "Attribute " << att << " is not found in any relation\n";
        }
        domainSize[att] = values[att].size();
        cout << "Attribute " << att << " size " << domainSize[att] << "\n";

    }
    
    // compute edges
    updateEdges();

    for(string s: allAttrs){
        computeStats(s);
    }
}

void Database::updateEdges(){
    for(Relation* r: relations){
        for(string att1: r->schema){
            for(string att2: r->schema){
                edges[att1].insert(att2);
            }
        }
    }
}


// given a set of attributes, the ids of relations to join, return the indicator projection
vector<Relation*> Database::getIndicator(vector<int> const& ids, unordered_set<string> const& attrs){
    vector<Relation*> indicators;

    // inside-outside algorithm
    // prebuild projection
    // find relations whose schema intersects with related relations
    for(unsigned int i = 0; i < this->relations.size(); i++){
        
        Relation* r = this->relations[i];

        if(find(ids.begin(), ids.end(), r->id) != ids.end()){
            // cout<<"skip because already in toJoin\n";
            continue;
        }

        vector<string> ordered_attrs = intersection(attrs, r->schema);
        if(ordered_attrs.size() == 0){
            continue;
        }

        // get projection
        string hash = "";
        hash.append(to_string(r->id));
        hash.append(":");
        for(string s: ordered_attrs){
            hash.append(s);
            hash.append(",");
        }
        // cout<<hash<<"\n";

        if(cache.find(hash) != cache.end()){
            // cout<<"hashed!\n";
        }else{
            unordered_set<string> keptAttr (ordered_attrs.begin(), ordered_attrs.end());
            Relation* projection = r->marginalizeKept(keptAttr);
            cache[hash] = projection;
        }
        indicators.push_back(cache[hash]);
    }
    return indicators;
}

void Database::computeStats(string attribute){
    // indices of relations 
    vector<int> idx = findRelatedRelations(attribute);

    // ids of relations to join
    vector<int> ids;
    for(int i: idx){
        ids.push_back(relations[i]->id);
    }

    vector<Relation*> ToJoin;

    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        ToJoin.push_back(relation);
    }

    // get the set of all attributes in ToJoin
    unordered_set<string> attrs;
    for(Relation* r : ToJoin){
        for(string attr: r->schema){
            attrs.insert(attr);
        }
    }
    clock_t start;
    double duration;
    start = clock();
    if(isindicatorprojection){
        getIndicator(ids, attrs);
    }
    indicduration += (clock() - start) / (double)CLOCKS_PER_SEC;
    
    computeSamplemarg(attribute);
    computeSamplejoin(attribute);
}

long long int Database::computeDegree(string attribute){
    long long int degree = 1;

    vector<int> idx = findRelatedRelations(attribute);

    unordered_set<string> attributes;

    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        for(string cur_attr : relation->schema){
            attributes.insert(cur_attr);
        }
    }

    for(string cur_attr : attributes){
        if(cur_attr == attribute){
            continue;
        }
        long long int pre_degree = degree;
        degree = degree * domainSize[cur_attr];
        
        if(degree/domainSize[cur_attr] != pre_degree){
            cout << attribute << "overflow!\n";
            return LLONG_MAX;
        }
    }

    cout << attribute <<" degree: " << degree <<"\n";
    
    return degree;
}


void Database::minDegree(){
    vector<string> leftAttrs(allAttrs);
//     cout<<"good here1\n";
    while(leftAttrs.size() > 0){
        unsigned int idx = -2;
        long long int degree = -1;
        for(unsigned int cur_idx = 0; cur_idx < leftAttrs.size(); cur_idx ++){
            long long int cur_degree = computeDegree(leftAttrs[cur_idx]);
            if (degree == -1 || cur_degree < degree){
                idx = cur_idx;
                degree = cur_degree;
            }
        }
        cout << degree <<"\n";
        eliminate(leftAttrs[idx]);
        leftAttrs.erase(leftAttrs.begin() + idx);
    }
    
//     unordered_set<string> leftAttrs(allAttrs.begin(), allAttrs.end());
//     while(leftAttrs.size() > 0){
//         string attr = "";
//         long long int degree = -1;
//         for(string cur_attr: leftAttrs){
//             long long int cur_degree = computeDegree(cur_attr);
//             if (degree == -1 || cur_degree < degree){
//                 attr = cur_attr;
//                 degree = cur_degree;
//             }
//         }
//         cout << degree <<"\n";
//         eliminate(attr);
//         leftAttrs.erase(attr);
//     }
}

long long int Database::computeFill(string attribute){
    long long int degree = 0;

    vector<int> idx = findRelatedRelations(attribute);

    // deduplicate
    unordered_set<string> attributes;
    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        for(string cur_attr : relation->schema){
            attributes.insert(cur_attr);
        }
    }
    attributes.erase(attribute);

    vector<string> attributes_vec;
    attributes_vec.insert(attributes_vec.end(), attributes.begin(), attributes.end());

    for(unsigned int i = 0; i < attributes_vec.size(); i ++){
        for(unsigned int j = i + 1; j < attributes_vec.size(); j ++){
            string att1 = attributes_vec[i];
            string att2 = attributes_vec[j];

            // if is a filling edge
            if(edges[att1].find(att2) == edges[att1].end()){
                degree += domainSize[att1] * domainSize[att2];
                
            }
        }
    }
//     cout<<degree << "\n";
    if(degree < 0){
        // cout << "overflow!\n";
        return LLONG_MAX;
    }
    
    return degree;
}

void Database::minFill(){
    unordered_set<string> leftAttrs(allAttrs.begin(), allAttrs.end());
    while(leftAttrs.size() > 0){
//         cout<< "leftAttrs size " << leftAttrs.size() <<"\n";
        string attr = "";
        long long int degree = -1;
        for(string cur_attr: leftAttrs){
            long long int cur_degree = computeFill(cur_attr);
            if (degree == -1 || cur_degree < degree){
                attr = cur_attr;
                degree = cur_degree;
            }
        }
        cout << degree <<"\n";
        eliminate(attr);
        leftAttrs.erase(attr);
        updateEdges();
    }
}

double Database::computeAGMjoin(string attribute){

    vector<int> idx = findRelatedRelations(attribute);

    // deduplicate
    unordered_set<string> attributes;
    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        for(string cur_attr : relation->schema){
            attributes.insert(cur_attr);
        }
    }

    vector<string> attributes_vec;
    attributes_vec.insert(attributes_vec.end(), attributes.begin(), attributes.end());
    SimplexSolver* solver = new SimplexSolver();

    solver->n = relations.size(), solver->m = attributes_vec.size();

    for(unsigned int i = 0; i < relations.size(); i++){
        solver->c[i] = -log(relations[i]->size());
    }

    for(unsigned int i = 0; i < attributes.size(); i++){
        solver->b[i] = -1;
        string cur_attr = attributes_vec[i];
        for(unsigned int j = 0; j < relations.size(); j++){
            Relation* relation = relations[j];
            // check if this relation contains attribute
            if(find(relation->schema.begin(), relation->schema.end(), cur_attr) != relation->schema.end()){
                // if contains
                solver->A[i][j] = 1;
            }else{
                solver->A[i][j] = 0;
            }
        }

    }
    
    pair<vector<double>, double> ret = solver->simplex();
    // cout  << attribute<<"\n";
    // cout  << -ret.second<<"\n";
    return -ret.second;
}

void Database::minAGMjoin(){
    vector<string> leftAttrs(allAttrs);
    while(leftAttrs.size() > 0){
        unsigned int idx = -2;
        double degree = -1;
        for(unsigned int cur_idx = 0; cur_idx < leftAttrs.size(); cur_idx ++){
            double cur_degree = computeAGMjoin(leftAttrs[cur_idx]);
            if (degree == -1 || cur_degree < degree){
                idx = cur_idx;
                degree = cur_degree;
            }
        }
        cout << degree <<"\n";
        eliminate(leftAttrs[idx]);
        leftAttrs.erase(leftAttrs.begin() + idx);
    }
}

double Database::computeAGMmarg(string attribute){

    vector<int> idx = findRelatedRelations(attribute);

    // deduplicate
    unordered_set<string> attributes;
    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        for(string cur_attr : relation->schema){
            attributes.insert(cur_attr);
        }
    }
    attributes.erase(attribute);
    if(attributes.size() == 0){
        return 0;
    }
    
    vector<string> attributes_vec;
    attributes_vec.insert(attributes_vec.end(), attributes.begin(), attributes.end());
    SimplexSolver* solver = new SimplexSolver();

    solver->n = relations.size(), solver->m = attributes_vec.size();

    for(unsigned int i = 0; i < relations.size(); i++){
        solver->c[i] = -log(relations[i]->size());
    }

    for(unsigned int i = 0; i < attributes.size(); i++){
        solver->b[i] = -1;
        string cur_attr = attributes_vec[i];
        for(unsigned int j = 0; j < relations.size(); j++){
            Relation* relation = relations[j];
            // check if this relation contains attribute
            if(find(relation->schema.begin(), relation->schema.end(), cur_attr) != relation->schema.end()){
                // if contains
                solver->A[i][j] = 1;
            }else{
                solver->A[i][j] = 0;
            }
        }

    }
    
    pair<vector<double>, double> ret = solver->simplex();
    // cout  << attribute<<"\n";
    // cout  << -ret.second<<"\n";
    return -ret.second;
}

void Database::minAGMmarg(){
    vector<string> leftAttrs(allAttrs);
    while(leftAttrs.size() > 0){
        unsigned int idx = -2;
        double degree = -1;
        for(unsigned int cur_idx = 0; cur_idx < leftAttrs.size(); cur_idx ++){
            double cur_degree = computeAGMmarg(leftAttrs[cur_idx]);
            if (degree == -1 || cur_degree < degree){
                idx = cur_idx;
                degree = cur_degree;
            }
        }
        cout << degree <<"\n";
        eliminate(leftAttrs[idx]);
        leftAttrs.erase(leftAttrs.begin() + idx);
    }
}

long long int Database::computeSamplejoin(string attribute){
    if(sample_rate == 0){
        return 0;
    }

    vector<int> idx = findRelatedRelations(attribute);

    // id of relations to join
    vector<int> ids;
    for(int i: idx){
        ids.push_back(relations[i]->id);
    }
    sort(ids.begin(), ids.end());

    string hash = "";
    for(int id: ids){
        hash.append(to_string(id));
        hash.append("-");
    }
    hash.append("join");
    
    if(sample_result_cache.find(hash) != sample_result_cache.end()){
//         cout<<"cached!";
//         cout << attribute << " estimated join size: " << sample_result_cache[hash] << "\n";
        return sample_result_cache[hash];
    }

    // find the largest relation
    int largest_idx = idx[0];
    int largest_size = relations[largest_idx]->size();

    for(unsigned int i = 1; i < idx.size(); i++){
        if(relations[idx[i]]->size() > largest_size){
            largest_idx = idx[i];
            largest_size = relations[largest_idx]->size();
        }
    }

    vector<Relation*> ToJoin;

    long long int sample_size = -1;
    long long int original_size = -1;
    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        // cout << "to join sample index " << idx[i] << "\n";
        Relation* relation = this->relations[idx[i]];
        if(largest_idx != idx[i]){
            ToJoin.push_back(relation);
        }else{
            Relation* sample = getSample(relation);
            sample_size = sample->size();
            original_size = relation->size();
            ToJoin.push_back(sample);
        }
    }


    // get the set of all attributes in ToJoin
    unordered_set<string> attrs;
    for(Relation* r : ToJoin){
        for(string attr: r->schema){
            // cout << "to join sample att " << attr << "\n";
            attrs.insert(attr);
        }
    }
    
    clock_t start;
    double duration;
    start = clock();
    if(isindicatorprojection){
        vector<Relation*> indicators = getIndicator(ids, attrs);

        for(Relation* r: indicators){
            // cout << "indicator sample id " << r->id << "\n";
            ToJoin.push_back(r);
        }
    }
    indicduration += (clock() - start) / (double)CLOCKS_PER_SEC;


    Operator op;
    Relation* joinResult = join(ToJoin);
//     cout << "join size: " << joinResult->size() << "\n";
//     cout << "original_size: " <<original_size<< "\n";
//     cout << "sample_size: " << sample_size<< "\n";
    long long int estimated_size = joinResult->size() * original_size / sample_size;
    
    if(estimated_size < 0){
        estimated_size = LLONG_MAX;
    }
    
    sample_result_cache[hash] = estimated_size;
    cout << attribute << " estimated join size: " << estimated_size << "\n";
    return estimated_size;
}

void Database::minSamplejoin(){
    clock_t start;
    double duration;
    vector<string> leftAttrs(allAttrs);
    while(leftAttrs.size() > 0){
        start = clock();
        unsigned int idx = -2;
        long long int degree = -1;
        for(unsigned int cur_idx = 0; cur_idx < leftAttrs.size(); cur_idx ++){
//             cout << "minsample join "<< leftAttrs[cur_idx]  << "\n";
            long long int cur_degree = computeSamplejoin(leftAttrs[cur_idx]);
            if (degree == -1 || cur_degree <= degree){
                
                idx = cur_idx;
                degree = cur_degree;
            }
        }
        cout << degree <<"\n";
        sampleduration += (clock() - start) / (double)CLOCKS_PER_SEC;
        
        eliminate(leftAttrs[idx]);
        leftAttrs.erase(leftAttrs.begin() + idx);
    
        duration = (clock() - start) / (double)CLOCKS_PER_SEC;
        cout << "time to eliminate: " << duration << "\n"; 
    }
}

long long int Database::computeSamplemarg(string attribute){
    if(sample_rate == 0){
        return 0;
    }

    vector<int> idx = findRelatedRelations(attribute);

    // id of relations to join
    vector<int> ids;
    for(int i: idx){
        ids.push_back(relations[i]->id);
    }
    sort(ids.begin(), ids.end());

    string hash = "";
    for(int id: ids){
        hash.append(to_string(id));
        hash.append("-");
    }
    hash.append(attribute);
    hash.append(":");
    hash.append("marg");
    
    if(sample_result_cache.find(hash) != sample_result_cache.end()){
        // cout << "marg cached \n";
        return sample_result_cache[hash];
    }
    string joinhash = "";
    for(int id: ids){
        joinhash.append(to_string(id));
        joinhash.append("-");
    }
    joinhash.append("join_relation");

    Relation* joinResult;
    long long int sample_size = -1;
    long long int original_size = -1;
    
    if(cache.find(joinhash) != cache.end()){
        // cout << "join cached \n";
        joinResult = cache[joinhash];
        sample_size = join_sample_rate[joinhash][0]; 
        original_size = join_sample_rate[joinhash][1];
    }else{
        // find the largest relation
        int largest_idx = idx[0];
        int largest_size = relations[largest_idx]->size();

        for(unsigned int i = 1; i < idx.size(); i++){
            if(relations[idx[i]]->size() > largest_size){
                largest_idx = idx[i];
                largest_size = relations[largest_idx]->size();
            }
        }

        vector<Relation*> ToJoin;
   
        // add these relations to the join list
        for(int i = idx.size() - 1; i >= 0; i--){
            // cout << "to join sample index " << idx[i] << "\n";
            Relation* relation = this->relations[idx[i]];
            if(largest_idx != idx[i]){
                ToJoin.push_back(relation);
            }else{
                Relation* sample = getSample(relation);
                sample_size = sample->size();
                original_size = relation->size();
                ToJoin.push_back(sample);
            }
        }

        // get the set of all attributes in ToJoin
        unordered_set<string> attrs;
        for(Relation* r : ToJoin){
            for(string attr: r->schema){
                // cout << "to join sample att " << attr << "\n";
                attrs.insert(attr);
            }
        }
        clock_t start;
        double duration;
        start = clock();
        if(isindicatorprojection){
            vector<Relation*> indicators = getIndicator(ids, attrs);

            for(Relation* r: indicators){
                // cout << "indicator sample id " << r->id << "\n";
                ToJoin.push_back(r);
            }
        }
        indicduration += (clock() - start) / (double)CLOCKS_PER_SEC;

        Operator op;
        joinResult = join(ToJoin);
        cache[joinhash] = joinResult;
        join_sample_rate[joinhash] = {sample_size, original_size} ;
    }

    // cout << "join size: " << joinResult->size() << "\n";
    Relation* margResult = marginalize(joinResult, attribute);

//     long long int estimated_size = margResult->size() * original_size / sample_size;
    long long int estimated_size = (int)margResult->marg_size((float)original_size / (float)sample_size);
    if(estimated_size < 0){
        estimated_size = LLONG_MAX;
    }
    sample_result_cache[hash] = estimated_size;
    
    cout << attribute << " estimated marg size: " << estimated_size  << "\n";
    

    return estimated_size;
}

void Database::minSamplemarg(){
    clock_t start;
    double duration;
    vector<string> leftAttrs(allAttrs);
    while(leftAttrs.size() > 0){
        start = clock();
//         cout<<"jojojo\n";
        unsigned int idx = -2;
        long long int degree = -1;
        for(unsigned int cur_idx = 0; cur_idx < leftAttrs.size(); cur_idx ++){
//             cout << "minsample join "<< leftAttrs[cur_idx]  << "\n";
            long long int cur_degree = computeSamplemarg(leftAttrs[cur_idx]);
            if (degree == -1 || cur_degree <= degree){
                
                idx = cur_idx;
                degree = cur_degree;
            }
        }
        
        cout << degree <<"\n";
        sampleduration += (clock() - start) / (double)CLOCKS_PER_SEC;
        eliminate(leftAttrs[idx]);
        leftAttrs.erase(leftAttrs.begin() + idx);
        duration = (clock() - start) / (double)CLOCKS_PER_SEC;
        cout << "time to eliminate: " << duration << "\n"; 
    }
}

void Database::eliminate(string attribute){

    cout << attribute <<"\n";
    // indices of relations 
    vector<int> idx = findRelatedRelations(attribute);

    // id of relations to join
    vector<int> ids;
    for(int i: idx){
        ids.push_back(relations[i]->id);
    }

    vector<Relation*> ToJoin;

    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        // cout << "to join index " << idx[i] << "\n";
        Relation* relation = this->relations[idx[i]];
        ToJoin.push_back(relation);
    }

    // remove these relations from original relations
    // iterate descendingly because erase early one will affect later ones
    for(int i = idx.size() - 1; i >= 0; i--){
        this->relations.erase(this->relations.begin() + idx[i]);
    }

    // get the set of all attributes in ToJoin
    unordered_set<string> attrs;
    for(Relation* r : ToJoin){
        for(string attr: r->schema){
            // cout << "to join att " << attr << "\n";
            attrs.insert(attr);
        }
    }
    
    bool single = (ids.size() == 1);
    // inside-outside algorithm
    // find relations whose schema intersects with related relations
    // for(unsigned int i = 0; i < this->relations.size(); i++){
        
    //     Relation* r = this->relations[i];

    //     if(find(idx.begin(), idx.end(), int(i)) != idx.end()){
    //         // cout<<"skip because already in toJoin\n";
    //         continue;
    //     }

    //     vector<string> ordered_attrs = intersection(attrs, r->schema);
    //     if(ordered_attrs.size() == 0){
    //         continue;
    //     }

    //     // get projection
    //     string hash = "";
    //     hash.append(to_string(r->id));
    //     hash.append(":");
    //     for(string s: ordered_attrs){
    //         hash.append(s);
    //         hash.append(",");
    //     }
    //     // cout<<hash<<"\n";

    //     if(cache.find(hash) != cache.end()){
    //         // cout<<"cached!\n";
    //     }else{
    //         // cout<<"new!!!!!!!!!!!!!\n";
    //         // cout<<hash<<"\n";
    //         unordered_set<string> keptAttr (ordered_attrs.begin(), ordered_attrs.end());
    //         Relation* projection = r->marginalizeKept(keptAttr);
    //         projection->indicator();
    //         cache[hash] = projection;
            
    //     }
    //     ToJoin.push_back(cache[hash]);
    // }
    clock_t start;
    double duration;
    start = clock();
    if(isindicatorprojection){
        
        
        vector<Relation*> indicators = getIndicator(ids, attrs);

        for(Relation* r: indicators){
            // cout << "indicator id " << r->id << "\n";
            ToJoin.push_back(r);
        }
        
    }
    indicduration += (clock() - start) / (double)CLOCKS_PER_SEC;
    
    Operator op;
    Relation* joinResult = join(ToJoin);
    
    cout << joinResult->size() << "\n";
    // cout << joinResult->trie->level << "\n";
    // if(attribute == "theme_id"){
    //     joinResult->print();
    // }


    Relation* margResult = marginalize(joinResult, attribute);
    
    if (single){
        margResult->id = ToJoin[0]->id;
    }
    // margResult->print();
    cout << margResult->size() << "\n";
    // if(attribute == "theme_id"){
    //     margResult->print();
    // }

    // for(Relation* r: ToJoin){
    //     delete r;
    // }

    // if(ToJoin.size() > 1){
    //     delete joinResult;
    // }

    this->relations.push_back(margResult);
}


void Database::Preprocess(){
    // GetNodeOrder();
    
    // for(Relation* relation: relations){
    //     buildTrie(relation);
    // }

    computeAllStats();
}