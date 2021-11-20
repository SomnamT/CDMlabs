#include "common.h"
#include "iostream"

using namespace Minisat;

bool lab1(int states[], int n, int button[][2], int m, bool answer[])
{
    Solver solver;
    Var variables[1000];
    int i, j;
    for(i = 0; i<m; ++i)
        variables[i] = solver.newVar();



    //TODO : your solution here.
    int stone[2000][2]={0};
    for(i = 0; i<m; i++)
    {
        for(j = 0; j<2; j++)
        if(stone[button[i][j]-1][0] == 0)
            stone[button[i][j]-1][0] = i+1;
        else stone[button[i][j]-1][1] = i+1;
    }
    for(i = 0; i<n; i++)
    {
        if(stone[i][1] == 0)
            if(states[i] == 0)
                solver.addClause(mkLit(variables[stone[i][0]-1]));
            else 
                solver.addClause(~mkLit(variables[stone[i][0]-1]));
        else if(states[i] == 0)
        {
            solver.addClause(~mkLit(variables[stone[i][0]-1]),~mkLit(variables[stone[i][1]-1]));
            solver.addClause(mkLit(variables[stone[i][0]-1]),mkLit(variables[stone[i][1]-1]));
        }
            else{
                solver.addClause(~mkLit(variables[stone[i][0]-1]),mkLit(variables[stone[i][1]-1]));
                solver.addClause(mkLit(variables[stone[i][0]-1]),~mkLit(variables[stone[i][1]-1]));
            }
    }


    //solve SAT problem
    auto sat = solver.solve();
    if(sat)
    {
        for(i = 0; i < m; ++i)
            answer[i] = (solver.modelValue(variables[i]) == l_True);
        return true;
    }
    return false;
}




