#pragma once
#include <iostream>
#include <vector>
using namespace std;
#define MAX_N 50
#define MAX_M 50

class SimplexSolver
{
public:
    int n, m;
    double A[MAX_M][MAX_N], b[MAX_M], c[MAX_N], v;
    int N[MAX_N], B[MAX_M]; // nonbasic & basic
    void pivot(int x, int y);
    int iterate_simplex();
    int initialise_simplex();
    pair<vector<double>, double> simplex();
};
