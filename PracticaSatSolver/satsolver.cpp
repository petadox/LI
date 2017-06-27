#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <time.h>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0

uint numVars;
uint numClauses;
vector<vector<int> > clauses;
vector<int> model;
vector<int> modelStack;
uint decisionLevel;

vector<vector<int> > cPositive;
vector<vector<int> > cNegative;

vector<float> puntuacions;

int decisiones, propagaciones;

void readClauses(){
    // Skip comments
    char c = cin.get();
    while (c == 'c') {
        while (c != '\n') c = cin.get();
        c = cin.get();
    }
    // Read "cnf numVars numClauses"
    string aux;
    cin >> aux >> numVars >> numClauses;
    clauses.resize(numClauses);
    cPositive.resize(numVars+1);
    cNegative.resize(numVars+1);
    puntuacions.resize(numVars+1,0);
    
    // Read clauses
    for (uint i = 0; i < numClauses; ++i) {
        int lit;
        while (cin >> lit and lit != 0) {
            clauses[i].push_back(lit);
            if (lit > 0) cPositive[lit].push_back(i);
            else cNegative[-lit].push_back(i);
            ++puntuacions[abs(lit)];
        }
    }
}

int currentValueInModel(int lit){
    if (lit >= 0) return model[lit];
    else {
        if (model[-lit] == UNDEF) return UNDEF;
        else return 1 - model[-lit];
    }
}

void setLiteralToTrue(int lit){
    modelStack.push_back(lit);
    if (lit > 0) model[lit] = TRUE;
    else model[-lit] = FALSE;
}

bool propagateGivesConflict (vector<int> litsToPropagate) {
    int numProp = litsToPropagate.size();
    for(uint j = 0; j < numProp; ++j) {
        if (litsToPropagate[j] > 0) {
			int negSize = cNegative[litsToPropagate[j]].size();
			for (uint i = 0; i < negSize; ++i) {
				bool someLitTrue = false;
				int numUndefs = 0;
				int lastLitUndef = 0;
				for (uint k = 0; not someLitTrue and k < clauses[cNegative[litsToPropagate[j]][i]].size(); ++k){
					int val = currentValueInModel(clauses[cNegative[litsToPropagate[j]][i]][k]);
					if (val == TRUE) someLitTrue = true;
					else if (val == UNDEF){ ++numUndefs; lastLitUndef = clauses[cNegative[litsToPropagate[j]][i]][k]; }
				}
				if (not someLitTrue and numUndefs == 0) {
					for (int s = 0; s < clauses[cNegative[litsToPropagate[j]][i]].size(); ++s) {
						++puntuacions[abs(clauses[cNegative[litsToPropagate[j]][i]][s])];
					}
					return true;
				}
				else if (not someLitTrue and numUndefs == 1) {
					setLiteralToTrue(lastLitUndef);
					++propagaciones;
					litsToPropagate.push_back(lastLitUndef);
					++numProp;
				}
			}
		}
		else {
			int posSize = cPositive[abs(litsToPropagate[j])].size();
			for (uint i = 0; i < posSize; ++i) {
				bool someLitTrue = false;
				int numUndefs = 0;
				int lastLitUndef = 0;
				for (uint k = 0; not someLitTrue and k < clauses[cPositive[abs(litsToPropagate[j])][i]].size(); ++k){
					int val = currentValueInModel(clauses[cPositive[abs(litsToPropagate[j])][i]][k]);
					if (val == TRUE) someLitTrue = true;
					else if (val == UNDEF){ ++numUndefs; lastLitUndef = clauses[cPositive[abs(litsToPropagate[j])][i]][k]; }
				}
				if (not someLitTrue and numUndefs == 0) {
					for (int s = 0; s < clauses[cPositive[abs(litsToPropagate[j])][i]].size(); ++s) {
						++puntuacions[abs(clauses[cPositive[abs(litsToPropagate[j])][i]][s])];
					}
					return true;
				}
				else if (not someLitTrue and numUndefs == 1) {
					setLiteralToTrue(lastLitUndef);
					++propagaciones;
					litsToPropagate.push_back(lastLitUndef);
					++numProp;
				}
			}
		}
    }
    return false;
}


void backtrack(){
    uint i = modelStack.size() -1;
    int lit = 0;
    while (modelStack[i] != 0){ // 0 is the DL mark
        lit = modelStack[i];
        model[abs(lit)] = UNDEF;
        modelStack.pop_back();
        --i;
    }
    // at this point, lit is the last decision
    modelStack.pop_back(); // remove the DL mark
    --decisionLevel;
    setLiteralToTrue(-lit);  // reverse last decision
}

// Heuristic for finding the next decision literal:
int getNextDecisionLiteral(){
    float maxim = 0.0;
    int lit;
    for (int i = 0; i < numVars+1; ++i) {
		if (model[i] == UNDEF and puntuacions[i] > maxim) {
			maxim = puntuacions[i];
			lit = i;
		}
	}
	return lit;
}

void checkmodel(){
    for (int i = 0; i < numClauses; ++i){
        bool someTrue = false;
        for (int j = 0; not someTrue and j < clauses[i].size(); ++j)
            someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
        if (not someTrue) {
            cout << "Error in model, clause is not satisfied:";
            for (int j = 0; j < clauses[i].size(); ++j) cout << clauses[i][j] << " ";
            cout << endl;
            exit(1);
        }
    }
}

int main(){
    clock_t t = clock();
    readClauses(); // reads numVars, numClauses and clauses
    model.resize(numVars+1,UNDEF);
    decisionLevel = 0;
    decisiones = 0;
    propagaciones = 0;
    vector<int> litsToPropagate(getNextDecisionLiteral());
    
    // DPLL algorithm
    while (true) {
		int decisionLit = getNextDecisionLiteral();
        if (decisionLit == 0) { checkmodel(); cout << (clock()-(float)t)/CLOCKS_PER_SEC << " SATISFIABLE" << endl << "DECISIONES " << decisiones << endl << "PROPAGACIONES " << propagaciones << endl; return 20; }
        // start new decision level:
        ++decisiones;
        modelStack.push_back(0);  // push mark indicating new DL
        ++decisionLevel;
        setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
        litsToPropagate[0] = decisionLit;
        while ( propagateGivesConflict(litsToPropagate) ) {
            if ( decisionLevel == 0) { cout << (clock()-(float)t)/CLOCKS_PER_SEC << " UNSATISFIABLE" << endl << "DECISIONES " << decisiones << endl << "PROPAGACIONES " << propagaciones << endl; return 10; }
            backtrack();
            litsToPropagate[0] = modelStack[modelStack.size()-1];
        }
    }
}
