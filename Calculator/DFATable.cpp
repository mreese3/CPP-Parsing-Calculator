#include "DFATable.h"

DFATable::DFATable(int myCount, int myInitialState, vector<DFAState> myStateList)
{
	count = myCount;
	initialState = myInitialState;
	stateList = myStateList;
}

void DFATable::printInfo()
{
	cout << "Count: " << count << " InitialState: " << initialState << endl;
	for (int i = 0; i < (signed)stateList.size(); i++)
		stateList[i].printInfo();
}
