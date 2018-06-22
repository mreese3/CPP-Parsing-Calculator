#include "DFAEdge.h"

DFAEdge::DFAEdge(int myCharSetIndex, int myTarget)
{
	charSetIndex = myCharSetIndex;
	target = myTarget;
}

void DFAEdge::printInfo()
{
	cout << "charSetIndex: " << charSetIndex << " target: " << target << endl;
}

