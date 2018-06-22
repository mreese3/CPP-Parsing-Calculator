#ifndef DFAEDGE_H
#define DFAEDGE_H

#include <iostream>
using namespace std;

class DFAEdge
{
public:
	int charSetIndex;
	int target;
	DFAEdge(int charSetIndex, int target);
	void printInfo();
};

#endif

