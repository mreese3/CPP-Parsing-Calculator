#include "DFAState.h"

DFAState::DFAState(int myIndex, int myEdgeCount, int myAcceptSymbol, vector<DFAEdge> myEdgeList)
{
	index = myIndex;
	edgeCount = myEdgeCount;
	acceptSymbol = myAcceptSymbol;
	edgeList = myEdgeList;
}

void DFAState::printInfo()
{
	cout << "Index: " << index << " Edge Count: " << edgeCount << " Accept Symbol: " << acceptSymbol << endl;
	for (int i = 0; i < (signed)edgeList.size(); i++)
		edgeList[i].printInfo();
}