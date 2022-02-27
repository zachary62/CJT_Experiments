#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "relation.h"
#include "database.h"
#include "operator.h"
#include <unordered_set>
#include "tree.h"


using namespace std;

void fav_junction(){

    vector<string> allAttrs {"date","store","item","unit_sales","onpromotion","oilprize","holiday_type","locale","locale_id","transferred","transactions","city","state","store_type","cluster", "family", "itemclass", "perishable", "item_data", "date_data"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/favorita/Sales.csv","Sales");
    jt.readRelation("data/favorita/Oil.csv","Oil");
    jt.readRelation("data/favorita/Holiday.csv","Holiday");
//     jt.readRelation("data/favorita/Holiday_select.csv","Holiday");
    jt.readRelation("data/favorita/Transactions.csv","Transactions");
//     jt.readRelation("data/favorita/Transactions_wf.csv","Transactions");
    jt.readRelation("data/favorita/Stores.csv","Stores");
//     jt.readRelation("data/favorita/Stores_wf.csv","Stores");
    jt.readRelation("data/favorita/Items.csv","Items");
//     jt.readRelation("data/favorita/Items_wf.csv","Items");
//     jt.readRelation("data/favorita/ItemAug.csv","ItemAug");
    jt.readRelation("data/favorita/DateAug.csv","DateAug");
    
    jt.addEdge("Oil", "Transactions");
    jt.addEdge("Stores", "Transactions");
    jt.addEdge("Holiday", "Transactions");
    jt.addEdge("Sales", "Transactions");
    jt.addEdge("Sales", "Items");
//     jt.addEdge("Items", "ItemAug");
    jt.addEdge("Holiday", "DateAug");

    if(!jt.checkAcyclic()){
        exit(0);
    }
    
    clock_t start;
    double duration;
    
    // calibration
//     jt.pull("Items","");
//     jt.push("Items","");
//     jt.pushdelete("Holiday", "");
//     jt.pull("Stores","");
//     jt.pull("Transactions","");
    jt.pull("Holiday","");
    
    start = clock();
//     jt.absorb("Stores",{});
//     jt.absorb("Stores",{"store"});
//     jt.absorb("Oil",{"oilprize"});
    
//     jt.absorb("Items",{});
//     jt.push("Items","");
    
//     jt.absorb("Stores",{});
//     jt.push("Stores","");
    
//     jt.absorb("Transactions",{});
//     jt.push("Transactions","");
    
//     jt.absorb("ItemAug",{});
    jt.absorb("DateAug",{});
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";

}


void lego_junction(){

    vector<string> allAttrs {"set_num","inventory_id","part_cat_id","color_id","theme_id","part_num","is_spare","is_trans","inventory_set_quantity","part_category_name","year","rgb", "color_name", "parent_id", "inventory_part_quantity", "theme_name", "num_parts", "set_name", "part_name", "color_data"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/lego/colors.csv","colors");
//     jt.readRelation("data/lego/colors_select.csv","colors");
//     jt.readRelation("data/lego/colors_wf.csv","colors");
    jt.readRelation("data/lego/inventory_parts.csv","inventory_parts");
    jt.readRelation("data/lego/inventory_sets.csv","inventory_sets");
    jt.readRelation("data/lego/part_categories.csv","part_categories");
    jt.readRelation("data/lego/parts.csv","parts");
    jt.readRelation("data/lego/sets.csv","sets");
    jt.readRelation("data/lego/themes.csv","themes");
//     jt.readRelation("data/lego/themes_wf.csv","themes");
    jt.readRelation("data/lego/colorAug.csv","colorAug");
        
    jt.addEdge("inventory_sets", "sets");
    jt.addEdge("sets", "themes");
    jt.addEdge("inventory_parts", "inventory_sets");
    jt.addEdge("colors", "inventory_parts");
    jt.addEdge("inventory_parts", "parts");
    jt.addEdge("parts", "part_categories");
    jt.addEdge("colors", "colorAug");
    
    if(!jt.checkAcyclic()){
        exit(0);
    }
    
    clock_t start;
    double duration;
    

//     jt.pull("inventory_sets","");
//     jt.push("inventory_sets","");
    
//     jt.pushdelete("colors", "");
//     jt.pull("inventory_sets","");
    jt.pull("colors","");
//     jt.pull("themes","");
    start = clock();
//     jt.absorb("inventory_parts",{});
//     jt.absorb("parts",{});
//     jt.absorb("sets",{"set_num"});
//     jt.absorb("part_categories",{"part_category_name"});
//     jt.absorb("colors",{});
//     jt.push("colors","");
//     jt.absorb("themes",{});
//     jt.push("themes","");
    jt.absorb("colorAug",{});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";

}

void tpc_junction(){

    vector<string> allAttrs {"date","time","customer","store","hdemo","ticket","item","brand", "moy", "dow", "category", "minute", "company", "year", "hour", "cdemo", "dom", "manufact","second", "qoy", "addr", "class","date_data", "add_data"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/tpc/store_sales.csv","store_sales");
    jt.readRelation("data/tpc/customer_address.csv","customer_address");
    jt.readRelation("data/tpc/customer_demographics.csv","customer_demographics");
//     jt.readRelation("data/tpc/customer.csv","customer");
    jt.readRelation("data/tpc/customer_wf.csv","customer");
    jt.readRelation("data/tpc/date_dim.csv","date_dim");
    jt.readRelation("data/tpc/household_demographics.csv","household_demographics");
    jt.readRelation("data/tpc/item.csv","item");
//     jt.readRelation("data/tpc/item_wf.csv","item");
    jt.readRelation("data/tpc/store.csv","store");
//     jt.readRelation("data/tpc/store_select.csv","store");
    jt.readRelation("data/tpc/time_dim.csv","time_dim");
//     jt.readRelation("data/tpc/dateAug.csv","dateAug");
    jt.readRelation("data/tpc/addressAug.csv","addressAug");
    
    jt.addEdge("date_dim", "store_sales");
    jt.addEdge("time_dim", "store_sales");
    jt.addEdge("customer", "store_sales");
    jt.addEdge("store", "store_sales");
    jt.addEdge("household_demographics", "store_sales");
    jt.addEdge("item", "store_sales");
    jt.addEdge("customer", "customer_address");
    jt.addEdge("customer", "customer_demographics");
//     jt.addEdge("date_dim", "dateAug");
    jt.addEdge("customer_address", "addressAug");
    
    if(!jt.checkAcyclic()){
        exit(0);
    }
    
//     jt.pull("store_sales","");
//     jt.push("store_sales","");
//     jt.pushdelete("store", "");
//     jt.pull("customer","");
//     jt.pull("date_dim","");
//     jt.pull("customer_address","");
    clock_t start;
    double duration;
    start = clock();
//     jt.absorb("date_dim",{});
//     jt.absorb("customer_address",{});
//     jt.absorb("store",{"store"});
//     jt.absorb("store",{});
//     jt.absorb("date_dim",{"year"});
//     jt.absorb("customer",{});
//     jt.push("customer",{});
//     jt.absorb("dateAug",{});
    jt.absorb("addressAug",{});
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";

}

void tpc_junction_union(){

    vector<string> allAttrs {"date","time","customer","store","hdemo","ticket","item","brand", "moy", "dow", "category", "minute", "company", "year", "hour", "cdemo", "dom", "manufact","second", "qoy", "addr", "class","date_data", "add_data"};

    Relation* R1 = new Relation("data/tpc/stream/store_sales1.csv",allAttrs);
    Relation* R2 = new Relation("data/tpc/stream/store_sales2.csv",allAttrs);

    clock_t start;
    double duration;
    start = clock();
    Relation* U = new Relation({"date","time","item","customer","store","hdemo","ticket"},allAttrs, false);
    U->add(R1);
//     U->add(R2);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";

}

void tpc_junction_lazy(){

    vector<string> allAttrs {"date","time","customer","store","hdemo","ticket","item","brand", "moy", "dow", "category", "minute", "company", "year", "hour", "cdemo", "dom", "manufact","second", "qoy", "addr", "class","date_data", "add_data"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/tpc/small_store_sales.csv","store_sales");
    jt.readRelation("data/tpc/customer_address.csv","customer_address");
    jt.readRelation("data/tpc/customer_demographics.csv","customer_demographics");
    jt.readRelation("data/tpc/customer.csv","customer");
    jt.readRelation("data/tpc/date_dim.csv","date_dim");
    jt.readRelation("data/tpc/household_demographics.csv","household_demographics");
    jt.readRelation("data/tpc/item.csv","item");
    jt.readRelation("data/tpc/store.csv","store");
    jt.readRelation("data/tpc/time_dim.csv","time_dim");

    jt.addEdge("date_dim", "store_sales");
    jt.addEdge("time_dim", "store_sales");
    jt.addEdge("customer", "store_sales");
    jt.addEdge("store", "store_sales");
    jt.addEdge("household_demographics", "store_sales");
    jt.addEdge("item", "store_sales");
    jt.addEdge("customer", "customer_address");
    jt.addEdge("customer", "customer_demographics");
    
    if(!jt.checkAcyclic()){
        exit(0);
    }
    

    clock_t start;
    double duration = 0;
    jt.pull("store_sales","");
    
//     for(int i = 1; i <= 30; i++){
//         jt.readRelation("data/tpc/stream/store_sales" + to_string(i) + ".csv","store_sales");
//         jt.pushdelete("store_sales","");
//         start = clock();
//         jt.push("store_sales","");
//         duration += (clock() - start) / (double)CLOCKS_PER_SEC;
//         cout << "time to read: " << duration << "\n";
//     }
    
    Relation* U = new Relation({"date","time","item","customer","store","hdemo","ticket"},allAttrs, false);
//     generated in python
    vector<vector<int>> randomread = {{7, 14, 29}, {7, 12, 19}, {5, 7, 14, 20, 23, 25, 26}, {5, 12, 16, 20, 28, 29}, {6, 22, 24, 30}, {1, 3, 10, 15, 27}};
    
    vector<string> dimensions = {"date_dim", "time_dim", "customer", "store", "household_demographics","item"};
    for(int j = 0; j < 6; j ++){
        int cur = 0;
        for(int i = 1; i <= 30; i++){
            if (cur == randomread[j].size()){
                break;
            }
            Relation* R = new Relation("data/tpc/stream/store_sales" + to_string(i) + ".csv",allAttrs);
            U->add(R);
            
            if(i == randomread[j][cur]){
                jt.relations["store_sales"] = U;
                jt.pushdelete("store_sales","");
                start = clock();
                jt.pull(dimensions[j],"");
                duration += (clock() - start) / (double)CLOCKS_PER_SEC;
                U = new Relation({"date","time","item","customer","store","hdemo","ticket"},allAttrs, false);
                cur ++;
            }

        }
    }
    
    cout << "time to read: " << duration << "\n";
    
}


void imdb_junction(){

    vector<string> allAttrs {"person_id","info_type_id2","person_name","movie_id","keyword_id","keyword","company_id","company_type_id","company", "info", "info_type_id", "title", "kind_id", "kind", "person_data", "company_data"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/imdb/cast_info.csv","cast_info");
    jt.readRelation("data/imdb/keyword.csv","keyword");
//     jt.readRelation("data/imdb/keyword_wf.csv","keyword");
    jt.readRelation("data/imdb/movie_keyword.csv","movie_keyword");
    jt.readRelation("data/imdb/name.csv","name");
//     jt.readRelation("data/imdb/name_wf.csv","name");
    jt.readRelation("data/imdb/person_info.csv","person_info");
    jt.readRelation("data/imdb/movie_companies.csv","movie_companies");
    jt.readRelation("data/imdb/company_name.csv","company_name");
    jt.readRelation("data/imdb/info_type.csv","info_type");
    jt.readRelation("data/imdb/movie_info.csv","movie_info");
    jt.readRelation("data/imdb/title.csv","title");
    jt.readRelation("data/imdb/kind_type.csv","kind_type");
//     jt.readRelation("data/imdb/personAug.csv","personAug");
//     jt.readRelation("data/imdb/companyAug.csv","companyAug");
    cout << "Finish reading the files.\n";
    
    jt.addEdge("movie_keyword", "title");
    jt.addEdge("movie_keyword", "keyword");
    jt.addEdge("title", "kind_type");
    jt.addEdge("cast_info", "title");
    jt.addEdge("cast_info", "person_info");
    jt.addEdge("movie_companies", "title");
    jt.addEdge("movie_companies", "company_name");
    jt.addEdge("name", "person_info");
    jt.addEdge("movie_info", "title");
    jt.addEdge("movie_info", "info_type");
//     jt.addEdge("name", "personAug");
//     jt.addEdge("company_name", "companyAug");
    
    
    if(!jt.checkAcyclic()){
        cout << "Not acyclic!\n";
        exit(0);
    }
    
    clock_t start;
    double duration;
    start = clock();
    jt.pull("name",{});
    jt.push("name","");
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time to calibrate: " << duration << "\n";
    
        
    start = clock();
    jt.absorb("name",{"person_id"});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time for Q1: " << duration << "\n";
    
    jt.readRelation("data/imdb/company_name_select.csv","company_name");
    jt.pushdelete("company_name", "");
    start = clock();
    jt.absorb("title",{"title"});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time for Q2: " << duration << "\n";
    
    jt.readRelation("data/imdb/company_name.csv","company_name");
    jt.readRelation("data/imdb/name_wf.csv","name");
    jt.pull("name",{});
    start = clock();
    jt.absorb("name",{});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time for 10 Person: " << duration << "\n";
    
    jt.readRelation("data/imdb/keyword_wf.csv","keyword");
    jt.pull("keyword",{});
    start = clock();
    jt.absorb("keyword",{});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time for 10 Movie: " << duration << "\n";
    
    jt.readRelation("data/imdb/personAug.csv","personAug");
    jt.readRelation("data/imdb/companyAug.csv","companyAug");
    jt.readRelation("data/imdb/keyword.csv","keyword");
    jt.readRelation("data/imdb/name.csv","name");
    
    jt.addEdge("name", "personAug");
    jt.addEdge("company_name", "companyAug");
    
    jt.pull("name",{});
    start = clock();
    jt.absorb("personAug",{});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time for Person Aug: " << duration << "\n";
    
    jt.pull("company_name",{});
    start = clock();
    jt.absorb("companyAug",{});
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "Time for Company Aug: " << duration << "\n";


}




void tri_junction(){

    vector<string> allAttrs {"A","B","C","D"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/tria/R.csv","R");
//     jt.readRelation("data/tria/Rone.csv","R");
//     jt.readRelation("data/tria/Rten.csv","R");
    jt.readRelation("data/tria/S.csv","S");
    jt.readRelation("data/tria/T.csv","T");
    jt.readRelation("data/tria/Taug.csv","Taug");
    jt.readEmptyRelation({"A","B","C"},"M");

    jt.addEdge("R", "M");
    jt.addEdge("S", "M");
    jt.addEdge("T", "M");
    jt.addEdge("T", "Taug");
    
    clock_t start;
    double duration;
    
    jt.pull("T",{});
    start = clock();

//     jt.absorb("M",{});
    jt.absorb("Taug",{});
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";
    
//     jt.push("M","");
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to read: " << duration << "\n";
    
//     start = clock();
//     jt.absorb("R",{});
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to read: " << duration << "\n";
    
    cout << "All good\n";
}

void triun_junction(){

    vector<string> allAttrs {"A","B","C","D"};
    JunctionTree jt(allAttrs);
    jt.readRelation("data/triaun/R.csv","R");
    jt.readRelation("data/triaun/S.csv","S");
    jt.readRelation("data/triaun/T.csv","T");
//     jt.readRelation("data/triaun/Tone.csv","T");
//     jt.readRelation("data/triaun/Tten.csv","T");
    jt.readRelation("data/tria/Taug.csv","Taug");
    jt.readEmptyRelation({"A","B","C"},"M");

    jt.addEdge("R", "M");
    jt.addEdge("S", "M");
    jt.addEdge("T", "M");
    jt.addEdge("T", "Taug");

    clock_t start;
    double duration;
    jt.pull("T",{});
    start = clock();

//     jt.absorb("M",{});
    jt.absorb("Taug",{});
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";
    
//     jt.push("M","");
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to read: " << duration << "\n";
    
//     start = clock();
//     jt.absorb("T",{});
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to read: " << duration << "\n";
    
    cout << "All good\n";
}

void test_data_cube(){
    clock_t start;
    double duration;

    vector<string> allAttrs {"A","B","C","D","E","F"};
    
    Relation* R1 = new Relation("data/datastructure/R1.csv", allAttrs);
    Relation* R2 = new Relation("data/datastructure/R2.csv", allAttrs);
    Relation* R3 = new Relation("data/datastructure/R3.csv", allAttrs);
    Relation* R4 = new Relation("data/datastructure/R4.csv", allAttrs);
    Relation* R5 = new Relation("data/datastructure/R5.csv", allAttrs);

    
    start = clock();
    vector<Relation*> relations = {R1, R2};

    Operator op;
    Relation* joinresult = op.join(relations, allAttrs);
    
    joinresult = op.join({joinresult, R3}, allAttrs);
    joinresult = op.join({joinresult, R4}, allAttrs);
    joinresult = op.join({joinresult, R5}, allAttrs);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    
    
    
    cout<< "size: " << joinresult->trie->size << "\n";
    cout << "time to join: " << duration << "\n";
}

void test_chain_calibrated(){
    clock_t start;
    double duration;

    vector<string> allAttrs {"A","B","C","D","E","F"};
    

    JunctionTree jt(allAttrs);
    jt.readRelation("data/datastructure/R1.csv","R1");
    jt.readRelation("data/datastructure/R2.csv","R2");
    jt.readRelation("data/datastructure/R3.csv","R3");
    jt.readRelation("data/datastructure/R4.csv","R4");
    jt.readRelation("data/datastructure/R5.csv","R5");

    jt.addEdge("R1", "R2");
    jt.addEdge("R2", "R3");
    jt.addEdge("R3", "R4");
    jt.addEdge("R4", "R5");


    start = clock();
    jt.pull("R1",{});
    jt.push("R1",{});
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to join: " << duration << "\n";
    jt.pushdelete("R5", "");
    start = clock();
    
    jt.absorb("R1",{});
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to join: " << duration << "\n";
}

void test_what_if(){
    clock_t start;
    double duration;
    start = clock();
    vector<string> allAttrs {"date","store","unit_sales","item","onpromotion","oilprize","holiday_type","locale","locale_id","transferred","transactions","city","state","store_type","cluster", "family", "itemclass", "perishable", "item_data", "date_data"};
    
    Relation* Sales = new Relation("data/favorita/Sales.csv", allAttrs);
    Relation* Oil = new Relation("data/favorita/Oil.csv", allAttrs);
    Relation* Holiday = new Relation("data/favorita/Holiday.csv", allAttrs);
    Relation* Transactions = new Relation("data/favorita/Transactions.csv", allAttrs);
    Relation* Stores = new Relation("data/favorita/Stores.csv", allAttrs);
    Relation* Items = new Relation("data/favorita/Items.csv", allAttrs);
    
    Relation* TransactionsWf = new Relation("data/favorita/Transactions-wf.csv", allAttrs);
    Relation* StoresWf = new Relation("data/favorita/Stores-wf.csv", allAttrs);
    Relation* ItemsWf = new Relation("data/favorita/Items-wf.csv", allAttrs);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read: " << duration << "\n";
    
//     start = clock();
//     unordered_set<string> s {"date"};
//     Relation* m1 = Oil->marginalizeKept(s);
//     Relation* m3 = Holiday->marginalizeKept(s);

//     s = {"store"};
//     Relation* m2 = Stores->marginalizeKept(s);
//     vector<Relation*> relations {m1, m2,m3,TransactionsWf};
//     s = {"store","date"};
//     Operator op;
//     Relation* m4 = op.joinAndMarginalize(relations, allAttrs, s);

//     relations = {m4, Sales};
//     s = {"item"};
//     Relation* m5 = op.joinAndMarginalize(relations, allAttrs, s);
    
//     relations = {m5, Items};
//     s = {};
//     op.joinAndMarginalize(relations, allAttrs, s);
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "transaction what if : " << duration << "\n";
//     start = clock();
    
//     start = clock();
//     unordered_set<string> s {"date"};
//     Relation* m1 = Oil->marginalizeKept(s);
//     Relation* m3 = Holiday->marginalizeKept(s);

//     s = {"store"};
//     Relation* m2 = StoresWf->marginalizeKept(s);
//     vector<Relation*> relations {m1, m2,m3,Transactions};
//     s = {"store","date"};
//     Operator op;
//     Relation* m4 = op.joinAndMarginalize(relations, allAttrs, s);
    

//     relations = {m4, Sales};
//     s = {"item"};
//     Relation* m5 = op.joinAndMarginalize(relations, allAttrs, s);
    
//     relations = {m5, Items};
//     s = {};
//     op.joinAndMarginalize(relations, allAttrs, s);
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "store what if : " << duration << "\n";
//     start = clock();
    Operator op;
    vector<Relation*> relations;
    start = clock();
    unordered_set<string> s {"date"};
    Relation* m1 = Oil->marginalizeKept(s);
    Relation* m3 = Holiday->marginalizeKept(s);

    s = {"store"};
    Relation* m2 = Stores->marginalizeKept(s);
    
    s = {"item"};
    Relation* m6 = ItemsWf->marginalizeKept(s);
    
    s = {"store","date"};
    relations = {m6, Sales};
    Relation* m7 = op.joinAndMarginalize(relations, allAttrs, s);
    
    
    
    relations = {m1, m2, m3, m7,Transactions};
    s = {};
    
    op.joinAndMarginalize(relations, allAttrs, s);
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "item what if : " << duration << "\n";
    start = clock();
    
    
}



int main(int argc, char *argv[])
{
    
//     test_calibrated();
//     test_marg();
//     test_what_if();
//     test_sample();
    
//     clock_t start;
//     double duration;
//     start = clock();
//     vector<string> allAttrs {"date", "store", "item", "unit_sales", "onpromotion", "city", "state", "store_type", "cluster"};
//     Relation* R = new Relation("data/favorita/Stores.csv", allAttrs);
    
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to read: " << duration << "\n";
    
//     start = clock();
//     R->trie->print();
    
    
// //     for(row r: *(R->relation)){
// //         row j =r;
// //     }
    
// //     for(row r: *(R->relation)){
// //         int i = 1;
// //     }
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout<< "size: " << R->trie->size << "\n";
//     cout << "time to iterate: " << duration << "\n";

    
    
    
//     clock_t start;
//     double duration;
//     start = clock();
// //     Relation* R = new Relation("data/small/T2.csv");
    
//     Relation* R = new Relation("data/favorita-small/Sales.csv");
// //     Relation* R = new Relation("data/imdb/cast_info4.csv");

//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to read: " << duration << "\n";
    
//     start = clock();
//     R->trie->print();
//     cout<< "size: " << R->trie->size << "\n";
    
// //     for(row r: *(R->relation)){
// //         row j =r;
// //     }
    
// //     for(row r: *(R->relation)){
// //         int i = 1;
// //     }
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to iterate: " << duration << "\n";
    
//     start = clock();
//     vector<bool> keptAttrs = {true,false, true, true, false};
//     Trie* marg_first = R->trie->marginalize(keptAttrs);
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "marginalize size: " << marg_first->size << "\n";
//     cout << "time to marginalize: " << duration << "\n";
    
//     start = clock();
//     marg_first->print();
//     duration = (clock() - start) / (double)CLOCKS_PER_SEC;
//     cout << "time to iterate: " << duration << "\n";
//     logo_example();
//     fav_example();
//     tpc_example();
//     imdb_example();
//     fav_junction();
//     lego_junction();
//     tpc_junction();
    imdb_junction();
//     tri_junction();
//     triun_junction();
//     tpc_junction_lazy();
//     tpc_junction_union();
//     cycle_syn(100, "marg",true);
//     cycle_syn(1000, "marg",true);
//     cycle_syn(100, "join",true);
//     cycle_syn(1000, "join",true);
    
//     cycle_syn(1000, "marg",true);
//     cycle_syn(1000, "marg",false);
//     cycle_syn(1000, "deg",true);
//     cycle_syn(1000, "deg",false);
//     cycle_syn(100, "agmm",true);
//     cycle_syn(100, "agmj",false);
//     cycle_syn(1000, "agmj",false);
    
//     cycle_syn(1000, "agmm",false);
//     cycle_syn(1000, "fill",true);
//     cycle_syn(1000, "fill",false);
//     cycle_syn(1000, "join",true);
//     cycle_syn(1000, "join",false);
    
    
//     star_syn(1000, "marg",true);
//     star_syn(1000, "marg",false);
//     star_syn(1000, "join",true);
//     star_syn(1000, "join",false);
//     star_syn(1000, "deg",true);
//     star_syn(1000, "deg",false);
//     star_syn(1000, "fill",true);
//     star_syn(1000, "fill",false);
//     star_syn(1000, "agmm",true);
//     star_syn(1000, "agmm",false);
//     star_syn(1000, "agmj",true);
//     star_syn(1000, "agmj",false);
    
    
    
    
//     test_data_cube();
//     test_chain_calibrated();
//     do 
//     {
//        cout << '\n' << "Press a key to continue...";
//     } while (cin.get() != '\n');
    
    
    // imdb_example();
    // test_marginalization(directory);
    // small_example();
    // join_example();
    // cout<<CLOCKS_PER_SEC;

}


