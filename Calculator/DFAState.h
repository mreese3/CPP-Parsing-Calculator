#ifndef DFASTATE_H
#define DFASTATE_H

#include <iostream>
#include <vector>
#include "DFAEdge.h"
using namespace std;

class DFAState
{
public:
	int index;
	int edgeCount;
	int acceptSymbol;
	vector<DFAEdge> edgeList;

	DFAState(int index, int edgeCount, int acceptSymbol, vector<DFAEdge> edgeList);
	void printInfo();
};

#endif

