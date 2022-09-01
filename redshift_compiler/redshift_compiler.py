import time
import pandas as pd
import numpy as np
import random
import statistics
import math
from dateutil import parser
from datetime import datetime
from functools import reduce

class joinGraph:
    def __init__(self, name):
        # store the table name -> schema 
        self.tables = dict()
        # store the join keys between two tables
        self.joins = dict()
        # store the message between two tables
        self.messages = dict()
        self.message_schema = dict()
        self.total_messages = 0 
        self.name = name
    
   
    def add_table(self, name: str, schema: list, replace = False):
        if name in self.tables and replace == False:
            raise Exception('There has already been a table named', name, 'If you want to replace the table, set replace = True.')
        self.tables[name] = schema
    
    def get_table(self, name: str):
        if name not in self.tables:
            raise Exception('There is no table named', name)
        return self.tables[name]

    
    def join(self, table_name_left: str, table_name_right: str, left_keys: list, right_keys: list):
        if len(left_keys) != len(right_keys):
            raise Exception('Join keys have different lengths!')
        if table_name_left not in self.tables:
            raise Exception(table_name_left, 'table doesn\'t exit!')
        if table_name_right not in self.tables:
            raise Exception(table_name_right, 'table doesn\'t exit!')
            
        table_left = self.tables[table_name_left]
        table_right = self.tables[table_name_right]
        
        #TODO check duplication for keys
        for left_key in left_keys:
            if left_key not in table_left:
                raise Exception(left_key, ' can\'t be found in table ', table_left)
        for right_key in right_keys:
            if right_key not in table_right:
                raise Exception(right_key, ' can\'t be found in table ', table_right)
        
#         left_keys = [table_name_left + "." + key for key in left_keys]
#         right_keys = [table_name_right + "." + key for key in right_keys]
        
        if table_name_left not in self.joins:
            self.joins[table_name_left] = dict()
        if table_name_right not in self.joins:
            self.joins[table_name_right] = dict()
        
        self.joins[table_name_left][table_name_right] = (left_keys, right_keys)
        self.joins[table_name_right][table_name_left] = (right_keys, left_keys)
    
    def check_acyclic(self):
        seen = set()
        
        # is this right?
        # counter example: A,B,C connects together
        # allows disconnected components?
        def dfs(table_name):
            seen.add(table_name)
            for neighbour in self.joins[table_name]:
                if neighbour in seen:
                    if table_name not in self.joins[neighbour]:
                        return False
                else:
                    dfs(neighbour)
            return True
        
        for table_name in self.joins:
            if table_name not in seen:
                if not dfs(table_name):
                    return False
        
        return True
    
    def calibration(self, root: str, groupby=[]):
        if not self.check_acyclic():
            raise Exception('Can\'t calibrate because not acyclic')
        
        if not root in self.tables:
            raise Exception('Can\'t find table ' + root)
        
        # if not cascade: ERROR: cannot drop materialized view tpcds_m6 because other objects depend on it Hint: Use DROP ... CASCADE to drop the dependent objects too.        
        for i in range(2*len(self.tables)):
            print("DROP materialized view IF EXISTS "+ self.name + "_m" + str(i) + " CASCADE;")
        
        def post_dfs(table_name, parent):
            for neighbour in self.joins[table_name]:
                if neighbour != parent:
                    post_dfs(neighbour, table_name)
            if parent is not None:
                self.send_message(table_name, parent, groupby)
        
        
        post_dfs(root, None)
        print("--downward message passing")
        def pre_dfs(table_name, parent):
            for neighbour in self.joins[table_name]:
                if parent is None or neighbour != parent:
                    self.send_message(table_name, neighbour, groupby)
                    pre_dfs(neighbour, table_name)
        
        pre_dfs(root, None)

    # send message from table1 to table2
    def send_message(self, table1, table2, groupby=[]):
        
        if table2 not in self.joins[table1]:
            raise Exception('Table', table1, 'and table', table2, 'are not connected')
        
        print("--sending message from", table1, "to", table2)
        
        if table1 not in self.messages:
            self.messages[table1] = dict()
        
        if table2 not in self.messages[table1]:
            self.messages[table1][table2] = self.name + "_m" + str(self.total_messages)
            self.total_messages += 1
        
        view_name = self.messages[table1][table2] 
        
        children = []
        for neighbour in self.joins[table1]:
            if neighbour != table2:
                children.append(neighbour)
        
        left_keys, right_keys = self.joins[table1][table2]
        
        kept_attributes = []
        # add message name because otherwise ambiguous
        kept_message_attributes = []
        
        # for each group-by not not in left_keys
        for att in groupby:
            if att not in left_keys:
                if att in self.tables[table1]:
                    kept_attributes.append(att)
                    kept_message_attributes.append(table1 + "." + att)
                else:
                    for child in children:
                        if att in self.message_schema[self.messages[child][table1]]:
                            kept_attributes.append(att)
                            kept_message_attributes.append(self.messages[child][table1] + "." + att)
        
        print("create materialized view " + view_name + " as")
        print("SELECT ", end = '')
        print(",".join([table1 + "." + key for key in left_keys] + kept_message_attributes), end = '')
        if len(children) == 0:
            print(",SUM(1)")
        else:
            print(",SUM(" + "*".join([self.messages[child][table1] + ".sum" for child in children]) + ")")
        
        print("FROM " + ",".join([self.messages[child][table1] for child in children] + [table1]))
        
        join_conditions = []
        
        for child in children:
            child_left_keys, child_right_keys = self.joins[child][table1]
            for i in range(len(child_left_keys)):
                join_conditions.append(self.messages[child][table1] + "." + child_left_keys[i] + " = " + table1 + "." + child_right_keys[i])
        
        if len(children) > 0:
            print("WHERE " + " AND ".join(join_conditions))
        print("GROUP BY " + ",".join([table1 + "." + key for key in left_keys] + kept_message_attributes ) + ";")
        
        self.message_schema[view_name] =  [table1 + "." + key for key in left_keys] + kept_attributes
        
        
        
        