import pandas as pd
import numpy as np
import random
import statistics
import math
from dateutil import parser
from datetime import datetime
from functools import reduce
import random
from random import choices
from sklearn.linear_model import LinearRegression
from sklearn.linear_model import Ridge
from sklearn.metrics import mean_squared_error
from sklearn.metrics import r2_score


def normalize(cov):
    cols = []
    
    if isinstance(cov, pd.DataFrame):
        cols = cov.columns
    else:
        cols = list(cov.axes[0])
        
    for col in cols:
        if col != 'cov:c':
            cov[col] = cov[col]/cov['cov:c']
    cov['cov:c'] = 1
    return cov

class agg_dataset:
    def set_meta(self, data, X, y, dimensions, name):
        self.data = data
        self.dimensions = dimensions
        self.y = y
        self.X =  [self.y] + X
        self.name = name
        self.datasets = set()
        self.datasets.add(self.name)
        
    def load_buyer(self, data, X, y, dimensions, name):
        self.data = data
        self.dimensions = dimensions
        self.X = X
        self.y = y
        self.name = name
        
        # don't impute y
        self.to_numeric(self.y, False)
        for x in self.X:
            self.to_numeric(x, False)
            
        self.X =  [self.y] + self.X
        
        dedup_dimensions = set()
        for d in dimensions:
            if isinstance(d, list):
                dedup_dimensions.update(d)
            else:
                dedup_dimensions.add(d)
        dedup_dimensions = list(dedup_dimensions)
        
        # project out attributes except x, y, dim
        self.data = self.data[self.X  + dedup_dimensions]
        
        self.datasets = set()
        self.datasets.add(self.name)
        
    def cov(self):
        return self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns))].sum()
    
    def compute_agg(self, norm = False):
        self.lift(self.name, self.X)
        
#         self.agg_dimensions = dict()
        
#         for d in self.dimensions:
#             if isinstance(d, list):
#                 self.agg_dimensions[tuple(d)] = self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns)) + d].groupby(d).sum().reset_index() 
#             else:
#                 self.agg_dimensions[d] = self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns)) + [d]].groupby(d).sum().reset_index() 
            
#         if norm:
#             for d in self.agg_dimensions.keys():
#                 self.agg_dimensions[d] = normalize(self.agg_dimensions[d])
            
#         self.covariance = normalize(self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns))].sum())
        
        self.X = [self.name + ':' + x for x in self.X]
        
    def group_by(self, d, inplace=True):
        gbdata = None
        if isinstance(d, list):
            gbdata = self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns)) + d].groupby(d).sum().reset_index() 
        else:
            gbdata = self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns)) + [d]].groupby(d).sum().reset_index() 
                
        if inplace:
            self.data = gbdata
        else:
            dataset = agg_dataset()
            dataset.data = gbdata
            dataset.X = self.X 
            return dataset
            
        

    
    def load_seller(self, data, dimensions, name, feature_transform = True, standardize = False):
        self.data = data
        self.dimensions = dimensions
        self.name = name
        
                
        dedup_dimensions = set()
        for d in dimensions:
            if isinstance(d, list):
                dedup_dimensions.update(d)
            else:
                dedup_dimensions.add(d)
        dedup_dimensions = list(dedup_dimensions)
        
        
        # find numeric attributes as features
        atts = []
        for att in self.data.columns:
            if att in dedup_dimensions:
                continue
            cond, col = self.is_numeric(att, 0.3, 2)
            if cond:
                self.data[att] = col
                if standardize:
                    self.data[att] = (self.data[att] - self.data[att].mean())/(self.data[att].std())
                
                atts.append(att)
                if feature_transform:
                    self.data["log" + att] = np.log(self.data[att])
                    self.data["sq" + att] = np.square(self.data[att])
                    self.data["cbr" + att] = np.cbrt(self.data[att])

                    atts.append("log" + att)
                    atts.append("sq" + att)
                    atts.append("cbr" + att)

        self.X = atts

        
        # project out attributes except x, y, dim
        self.data = self.data[self.X + dedup_dimensions]

    def is_numeric(self, att, impute_rate, cardinality):
        col = pd.to_numeric(self.data[att],errors="coerce")
        nan_count = sum(np.isnan(col))
        unique_count = len(col.unique())
        if nan_count/len(self.data) < impute_rate and unique_count/len(self.data) < cardinality:
            mean_value = col.mean()
            col.fillna(value=mean_value, inplace=True)
            return True, col
        else:
            return False, col
    
    def to_numeric(self, att, impute=True, impute_rate = 1):
        # parse attribute to numeric
        self.data[att] = pd.to_numeric(self.data[att],errors="coerce")
        # count the number of nan
        nan_count = sum(np.isnan(self.data[att]))
        
        if impute:
            # impute error only if missing rate is not above threshold
            if nan_count/len(self.data) < impute_rate:
                mean_value=self.data[att].mean()
                self.data[att].fillna(value=mean_value, inplace=True)
                return True
            else:
                return False
        else:
            # else, remove records with missing value
            self.data = self.data[~np.isnan(self.data[att])]
    
    def lift(self, tablename, attributes):
        self.data['cov:c'] = 1

        for i in range(len(attributes)):
            for j in range(i, len(attributes)):
                self.data['cov:Q:' + tablename + ":" + attributes[i] + ","+ tablename + ":" + attributes[j]] = self.data[attributes[i]] * self.data[attributes[j]]

        for attribute in attributes:
            self.data= self.data.rename(columns = {attribute:'cov:s:' + tablename + ":" + attribute})
    
    def absorb(self, agg_data, dimension, attrs):
        
        if agg_data.name in self.datasets:
            print("already absorbed this data")
            return
            
        self.data = connect(self, agg_data, dimension, True, attrs)
        
        # update the group-by for dimensions
        for d in self.dimensions:
            if isinstance(d, list):
                self.agg_dimensions[tuple(d)] = self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns)) + d].groupby(d).sum()
            else:
                self.agg_dimensions[d] = self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns)) + [d]].groupby(d).sum()
            
        self.covariance = normalize(self.data[list(filter(lambda col: col.startswith("cov:"), self.data.columns))].sum())
        
        self.X = self.X + attrs
        self.datasets.add(agg_data.name)
    
    # like absorb, but return a new agg_dataset without dim
    def join(self, agg_data, dimension, attrs):
        dataset = agg_dataset()
        dataset.data = connect(self, agg_data, dimension, True, attrs)
        dataset.X = self.X + agg_data.X
        return dataset
    
# return the coefficients of features and a constant 
def linear_regression(cov_matrix, features, result):
    a = np.empty([len(features) + 1, len(features) + 1])
    b = np.empty(len(features) + 1)
    
    for i in range(len(features)):
        for j in range(len(features)):
            if 'cov:Q:' + features[i] + ","+ features[j] in cov_matrix:
                a[i][j] = cov_matrix['cov:Q:' + features[i] + ","+ features[j]]
            else:
                a[i][j] = cov_matrix['cov:Q:' + features[j] + ","+ features[i]]
    
    for i in range(len(features)):
        a[i][len(features)] = cov_matrix['cov:s:' + features[i]]
        a[len(features)][i] = cov_matrix['cov:s:' + features[i]]
        if 'cov:Q:' + result + "," + features[i] in cov_matrix:
            b[i] = cov_matrix['cov:Q:' + result + "," + features[i]]
        else:
            b[i] = cov_matrix['cov:Q:' + features[i] + "," + result]
    
    b[len(features)] = cov_matrix['cov:s:' + result]
    
    a[len(features)][len(features)] = cov_matrix['cov:c']
#     print(a,b)
    return np.linalg.solve(a, b)

def square_error(cov_matrix, features, result, parameter):
    se = cov_matrix['cov:Q:'  + result + "," + result]
    
#     print(se)
    for i in range(len(features)):
        for j in range(len(features)):
            if 'cov:Q:'  + features[i] + "," + features[j] in cov_matrix:
                se += parameter[i] * parameter[j] * cov_matrix['cov:Q:'  + features[i] + "," + features[j]]
            else:    
                se += parameter[j] * parameter[i] * cov_matrix['cov:Q:'  + features[j] + "," + features[i]]
#             print(se, 'cov:Q:'  + features[i] + "," + features[j])
   
    
    for i in range(len(features)):
        se += 2 * parameter[i] * parameter[-1] * cov_matrix['cov:s:'  + features[i]]
        if 'cov:Q:' + result + "," + features[i] in cov_matrix:
            se -= 2 * parameter[i] *  cov_matrix['cov:Q:' + result + "," + features[i]]
        else:
            se -= 2 * parameter[i] *  cov_matrix['cov:Q:' + features[i] + "," + result]
    
#     print(se)
    se -= 2 * parameter[-1] * cov_matrix['cov:s:'  + result]
    se += cov_matrix['cov:c'] * parameter[-1] * parameter[-1]

    return se

def total_sum_of_square(cov_matrix, result):
    return cov_matrix['cov:Q:'  + result + "," + result] - cov_matrix['cov:s:'  + result] * cov_matrix['cov:s:'  + result] / cov_matrix['cov:c']

def mean_squared_error(cov_matrix, features, result, parameter):
    return square_error(cov_matrix, features, result, parameter)/cov_matrix['cov:c']


def r2(cov_matrix, features, result, parameter):
    result =  1 - square_error(cov_matrix, features, result, parameter)/total_sum_of_square(cov_matrix, result)
    if result > 2:
        # overflow
        return -1
    return result

def adjusted_r2(cov_matrix, features, result, parameter):
    return 1 - (cov_matrix['cov:c']-1)*(1 - r2(cov_matrix, features, result, parameter))/(cov_matrix['cov:c'] - len(parameter) - 1)

def connect(aggdata1, aggdata2, dimension, left_inp = False, right_attrs = []):
    
    if isinstance(dimension, list):
        dimension = tuple(dimension)
    
#     if left_inp:
#         agg1 = aggdata1.data
#     else:
#         agg1 = aggdata1.agg_dimensions[dimension]
        
#     agg2 = aggdata2.agg_dimensions[dimension]

    agg1 = aggdata1.data
    agg2 = aggdata2.data
    
    left_attributes = aggdata1.X
    right_attributes = aggdata2.X
    

    join = pd.merge(agg1, agg2, how='inner', left_on=dimension, right_on=dimension)
#     print(join)
#         join = pd.merge(agg1, agg2, how='inner', left_index=True, right_index=True)
#     join = join.drop('cov:c_x', 1)
#     join = join.rename(columns = {'cov:c_y':'cov:c'})
    join['cov:c'] = join['cov:c_x'] * join['cov:c_y']  

    

    
    for i in range(len(right_attributes)):
        for j in range(i, len(right_attributes)):
            if 'cov:Q:' + right_attributes[i] + "," + right_attributes[j] in join:
                join['cov:Q:' + right_attributes[i] + "," + right_attributes[j]] *= join['cov:c_x']
            else:
                join['cov:Q:' + right_attributes[j] + "," + right_attributes[i]] *= join['cov:c_x']
                
    for i in range(len(left_attributes)):
        for j in range(i, len(left_attributes)):
            if 'cov:Q:' + left_attributes[i] + "," + left_attributes[j] in join:
                join['cov:Q:' + left_attributes[i] + "," + left_attributes[j]] *= join['cov:c_y']
            else:
                join['cov:Q:' + left_attributes[j] + "," + left_attributes[i]] *= join['cov:c_y']
            
    
    
    for att1 in left_attributes:
        for att2 in right_attributes:
            if 'cov:Q:' + att1 + "," + att2 in join:
                join['cov:Q:' + att1 + "," + att2] = join['cov:s:' + att1] * join['cov:s:' + att2]
            else:
                join['cov:Q:' + att2 + "," + att1] = join['cov:s:' + att2] * join['cov:s:' + att1]
                
    # fill in nan
    for att1 in left_attributes:
        join['cov:s:' + att1] *= join['cov:c_y']
    for att2 in right_attributes:
        join['cov:s:' + att2] *= join['cov:c_x']
    
    join = join.drop('cov:c_x', 1)
    join = join.drop('cov:c_y', 1)
    
    return join

def select_features(train, test, seller, dimension, k):
    join_test = connect(test, seller, dimension)
    join_train = connect(train, seller, dimension)

    cur_atts = []
    join_train_cov = join_train.sum()
    join_test_cov = join_test.sum()
    final_r2 = 0
    
    for i in range(k):
        best_r2 = 0
        best_att = -1
        for att in train.X + seller.X:
            if att in cur_atts or att == train.name + ":" + train.y:
                continue
            # maybe singular
            try:
                parameter = linear_regression(join_train_cov, cur_atts + [att], train.name + ":" + train.y)
            except:
                continue
            cur_r2 = r2(join_test_cov, cur_atts + [att], train.name + ":" + train.y, parameter)
    #         print(cur_r2, att)
            if cur_r2 > best_r2:
                best_r2 = cur_r2
                best_att = att
        if best_r2 == 0 or best_r2 < final_r2:
            break
        cur_atts = cur_atts + [best_att]
        final_r2 = best_r2
#         print(i, best_r2, cur_atts)
    return cur_atts, final_r2


# return the coefficients of features and a constant 
def ridge_linear_regression(cov_matrix, features, result, alpha):
    a = np.empty([len(features) + 1, len(features) + 1])
    b = np.empty(len(features) + 1)
    
    for i in range(len(features)):
        for j in range(len(features)):
            if 'cov:Q:' + features[i] + ","+ features[j] in cov_matrix:
                a[i][j] = cov_matrix['cov:Q:' + features[i] + ","+ features[j]]
            else:
                a[i][j] = cov_matrix['cov:Q:' + features[j] + ","+ features[i]]
        if i == j:
            a[i][i] += alpha
    
    for i in range(len(features)):
        a[i][len(features)] = cov_matrix['cov:s:' + features[i]]
        a[len(features)][i] = cov_matrix['cov:s:' + features[i]]
        if 'cov:Q:' + result + "," + features[i] in cov_matrix:
            b[i] = cov_matrix['cov:Q:' + result + "," + features[i]]
        else:
            b[i] = cov_matrix['cov:Q:' + features[i] + "," + result]
    
    b[len(features)] = cov_matrix['cov:s:' + result]
    
    a[len(features)][len(features)] = cov_matrix['cov:c']
#     print(a,b)
    return np.linalg.solve(a, b)