#ifndef DFATABLE_H
#define DFATABLE_H

#include "DFAState.h"
#include "vector"

class DFATable
{
public:
	int count;
	int initialState;
	vector<DFAState> stateList;

	DFATable(int count, int initialState, vector<DFAState> stateList);
	void printInfo();
};

#endif

