#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <stack>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>
#include "DFATable.h"
#include "DFAState.h"
#include "DFAEdge.h"
#include <iomanip>
#include <msclr/marshal_cppstd.h>
#using <System.Xml.dll>

using namespace std;
using namespace System;
using namespace System::Xml;

struct Cell
{
	int action;
	int value;
};

struct Production
{
	int left;
	vector<int> rightList;
};

struct Symbol
{
	string name;
	int type;
};

struct CharSet
{
	vector<int> unicodeList;
};

template <typename T> void printVector(T const& coll)
{
	typename T::const_iterator pos;  // iterator to iterate over coll
	typename T::const_iterator end(coll.end());  // end position

	for (pos = coll.begin(); pos != end; ++pos) {
		cout << *pos << ' ';
	}
	cout << endl;
}

void printStack(stack<int> myStack)
{
	stack<int> tempStack;
	while (myStack.size() >= 1)
	{
		tempStack.push(myStack.top());
		myStack.pop();
	}

	cout << "[";
	while (tempStack.size() >= 1)
	{
		cout << tempStack.top();
		if (tempStack.size() != 1) cout << ",";
		tempStack.pop();
	}
	cout << "]" << endl;
}

void printStack(stack<string> myStack)
{
	stack<string> tempStack;
	while (myStack.size() >= 1)
	{
		tempStack.push(myStack.top());
		myStack.pop();
	}

	cout << "[";
	while (tempStack.size() >= 1)
	{
		cout << tempStack.top();
		if (tempStack.size() != 1) cout << ",";
		tempStack.pop();
	}
	cout << "]" << endl;
}

bool fileExists(string filename)
{
	ifstream ifile(filename);
	return (bool)ifile;
}

string fileToString(string filename)
{
	ifstream inFile;
	inFile.open(filename);
	stringstream strStream;
	strStream << inFile.rdbuf();
	string str = strStream.str();

	return str;
}

bool inVector(vector<int> v, int num)
{
	if (find(v.begin(), v.end(), num) != v.end())
		return true;
	else
		return false;
}

int getCharSet(DFATable dt, vector<CharSet> charTable, int origState, int code)
{
	vector<DFAState> stateList = dt.stateList;
	vector<DFAEdge> edgeList = {};

	for (unsigned int i = 0; i < stateList.size(); i++)
	{
		DFAState currState = stateList[i];
		edgeList = currState.edgeList;

		int currStateIndex = currState.index;
		if (currStateIndex == origState)
			break;
	}

	vector<int> possibleCharList = {};
	for (unsigned int i = 0; i < edgeList.size(); i++)
	{
		DFAEdge currEdge = edgeList[i];
		int charSetIndex = currEdge.charSetIndex;
		possibleCharList.push_back(charSetIndex);
	}
 
	int charSetIndex = -1;
	for (unsigned i = 0; i < possibleCharList.size(); i++)
	{
		int currCharSet = possibleCharList[i];
		vector<int> unicodeList = charTable[currCharSet].unicodeList;
		if (inVector(unicodeList, code))
		{
			charSetIndex = currCharSet;
			break;
		}
	}

	return charSetIndex;
}

int applyProduction(DFATable dt, int origStateIndex, int charSetIndex)
{
	int returnVal = -1;
	vector<DFAState> stateList = dt.stateList;
	for (int i = 0; i < (signed)stateList.size(); i++)
	{
		DFAState currState = stateList[i];

		if (currState.index == origStateIndex)
		{
			vector<DFAEdge> edgeList = currState.edgeList;
			for (int j = 0; j < (signed)edgeList.size(); j++)
			{
				DFAEdge currEdge = edgeList[j];
				int edgeCharSetIndex = currEdge.charSetIndex;
				int edgeTarget = currEdge.target;
				if (edgeCharSetIndex == charSetIndex)
					returnVal = edgeTarget;
			}
		}
	}
	return returnVal;
}

bool isWhiteSpace(string str)
{
	bool returnVal = true;
	for (int i = 0; i <= (signed)(str.length() - 1); i++)
	{
		if (isspace(str[i]) == false)
		{
			returnVal = false;
			break;
		}
	}
	return returnVal;
}

int getAcceptSymbolIndex(DFATable dt, int origIndex)
{
	vector<DFAState> stateList = dt.stateList;
	int returnVal;
	for (int i = 0; i < (signed)stateList.size(); i++)
	{
		DFAState currState = stateList[i];

		if (currState.index == origIndex)
		{
			returnVal = currState.acceptSymbol;
			break;
		}
	}
	return returnVal;
}

string removeExtension(const string& filename) 
{
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == string::npos) return filename;
	return filename.substr(0, lastdot);
}

struct TokenData
{
	vector<string> tokenList;
	vector<int> symbolList;
};

TokenData tokenizeFile(vector<CharSet> charTable, vector<Symbol> symbolTable, DFATable dt, string filename)
{
	string str = fileToString(filename);
	int currState = dt.initialState;
	int targetState;
	int acceptSymbolIndex;
	string token = "";

	vector<string> tokenList = {};
	vector<int> symbolList = {};

	int i = 0;
	while(true)
	{
		int code = str[i];
		char currChar = str[i];
		int currSet = getCharSet(dt, charTable, currState, code);

		if (currSet != -1)
		{
			targetState = applyProduction(dt, currState, currSet);
			currState = targetState;
			token = token + currChar;
			i = i + 1;
		}
		else if (currSet == -1 && currState != dt.initialState)
		{
			if (isWhiteSpace(token) == false)
			{
				acceptSymbolIndex = getAcceptSymbolIndex(dt, currState);
				tokenList.push_back(token);
				symbolList.push_back(acceptSymbolIndex);
			}
			token = "";
			currState = dt.initialState;
		}
		else
		{
			tokenList.push_back(string(1, str[i]));
			symbolList.push_back(1);
			i = i + 1;
		}

		if (i >= (signed)str.length())
		{
			if (isWhiteSpace(token) == false)
			{
				acceptSymbolIndex = getAcceptSymbolIndex(dt, currState);
				tokenList.push_back(token);
				symbolList.push_back(acceptSymbolIndex);
			}
				break;
		}
	}

	TokenData result;
	result.tokenList = tokenList;
	result.symbolList = symbolList;
	return result;
}

DFATable buildDFATable(XmlDocument^ doc)
{
	vector<DFAState> stateVector = {};

	XmlNode^ dfaTableNode = doc->SelectSingleNode("//DFATable");
	int count = Convert::ToInt32(dfaTableNode->Attributes["Count"]->Value);
	int initState = Convert::ToInt32(dfaTableNode->Attributes["InitialState"]->Value);

	XmlNodeList^ stateList = dfaTableNode->ChildNodes;
	for (int i = 0; i <= stateList->Count - 1; i++)
	{
		XmlNode^ stateNode = stateList[i];

		int index = Convert::ToInt32(stateNode->Attributes["Index"]->Value);
		int edgeCount = Convert::ToInt32(stateNode->Attributes["EdgeCount"]->Value);
		int acceptSymbol = Convert::ToInt32(stateNode->Attributes["AcceptSymbol"]->Value);
		vector<DFAEdge> edgeVector = {};

		XmlNodeList^ edgeList = stateNode->ChildNodes;
		for (int j = 0; j <= edgeList->Count - 1; j++)
		{
			XmlNode^ edgeNode = edgeList[j];
			int charSetIndex = Convert::ToInt32(edgeNode->Attributes["CharSetIndex"]->Value);
			int target = Convert::ToInt32(edgeNode->Attributes["Target"]->Value);
			edgeVector.push_back(DFAEdge(charSetIndex, target));
		}

		stateVector.push_back(DFAState(index, edgeCount, acceptSymbol, edgeVector));
	}

	return DFATable(count, initState, stateVector);
}

void printCharTable(vector<CharSet> v)
{
	for (unsigned int i = 0; i <= v.size() - 1; i++)
	{
		vector<int> unicodeList = v[i].unicodeList;
		cout << "Index: " << i << " unicodeList: ";
		for (unsigned int j = 0; j <= unicodeList.size() - 1; j++)
			cout << unicodeList[j] << " ";
		cout << endl;
	}
}

vector<CharSet> buildCharTable(XmlDocument^ doc)
{
	XmlNode^ mCharSet = doc->SelectSingleNode("//m_CharSet");
	int count = Convert::ToInt32(mCharSet->Attributes["Count"]->Value);
	vector<CharSet> charTable(count);

	XmlNodeList^ charSetNodeList = mCharSet->ChildNodes;
	for (int i = 0; i <= charSetNodeList->Count - 1; i++)
	{
		XmlNode^ charSetNode = charSetNodeList[i];
		int index = Convert::ToInt32(charSetNode->Attributes["Index"]->Value);
		int count = Convert::ToInt32(charSetNode->Attributes["Count"]->Value);

		vector<int> unicodeList = {};
		XmlNodeList^ charNodeList = charSetNode->ChildNodes;
		for (int j = 0; j <= charNodeList->Count - 1; j++)
		{
			XmlNode^ charNode = charNodeList[j];
			int unicodeIndex = Convert::ToInt32(charNode->Attributes["UnicodeIndex"]->Value);
			unicodeList.push_back(unicodeIndex);
		}

		charTable[i].unicodeList = unicodeList;
	}

	return charTable;
}

void printSymbolTable(vector<Symbol> v)
{
	for (unsigned int i = 0; i <= v.size() - 1; i++)
	{
		string name = v[i].name;
		int type = v[i].type;
		cout << "name: " << name << " type: " << type << endl;
	}
}

vector<Symbol> buildSymbolTable(XmlDocument^ doc)
{
	XmlNode^ mSymbol = doc->SelectSingleNode("//m_Symbol");
	int count = Convert::ToInt32(mSymbol->Attributes["Count"]->Value);
	vector<Symbol> symbolTable(count);

	XmlNodeList^ symbolNodeList = mSymbol->ChildNodes;
	for (int i = 0; i <= symbolNodeList->Count - 1; i++)
	{
		XmlNode^ symbolNode = symbolNodeList[i];
		int index = Convert::ToInt32(symbolNode->Attributes["Index"]->Value);
		string name = msclr::interop::marshal_as<string>(symbolNode->Attributes["Name"]->Value);
		int type = Convert::ToInt32(symbolNode->Attributes["Type"]->Value);

		symbolTable[i].name = name;
		symbolTable[i].type = type;
	}

	return symbolTable;
}

void printParseTable(vector<vector<Cell>> v)
{
	int rows = v.size();
	int cols = v[0].size();

	for (int i = 0; i <= rows - 1; i++)
	{
		for (int j = 0; j <= cols - 1; j++)
		{
			int action = v[i][j].action;
			int value = v[i][j].value;
			cout << "[" << action << "," << value << "] ";
		}
		cout << endl;
	}
}

vector<vector<Cell>> buildLALRTable(XmlDocument^ doc, int symbolCount)
{
	XmlNode^ LALRTable = doc->SelectSingleNode("//LALRTable");
	int count = Convert::ToInt32(LALRTable->Attributes["Count"]->Value);

	int rows = count;
	int cols = symbolCount;
	vector<vector<Cell>> parseTable;
	parseTable.resize(rows, vector<Cell>(cols));

	XmlNodeList^ LALRNodeList = LALRTable->ChildNodes;
	for (int i = 0; i <= LALRNodeList->Count - 1; i++)
	{
		XmlNode^ LALRState = LALRNodeList[i];
		int index = Convert::ToInt32(LALRState->Attributes["Index"]->Value);
		int actionCount = Convert::ToInt32(LALRState->Attributes["ActionCount"]->Value);
		XmlNodeList^ LALRActionList = LALRState->ChildNodes;
		for (int j = 0; j <= LALRActionList->Count - 1; j++)
		{
			XmlNode^ LALRAction = LALRActionList[j];
			int symbolIndex = Convert::ToInt32(LALRAction->Attributes["SymbolIndex"]->Value);
			int action = Convert::ToInt32(LALRAction->Attributes["Action"]->Value);
			int value = Convert::ToInt32(LALRAction->Attributes["Value"]->Value);

			parseTable[i][symbolIndex].action = action;
			parseTable[i][symbolIndex].value = value;
		}
	}

	return parseTable;
}

void printProdTable(vector<Production> v)
{
	for (unsigned int i = 0; i <= v.size() - 1; i++)
	{
		int left = v[i].left;
		vector<int> rightList = v[i].rightList;
		cout << "left: " << left << " right: ";
		for (unsigned int j = 0; j <= rightList.size() - 1; j++)
			cout << rightList[j] << " ";
		cout << endl;
	}
}

vector<Production> buildProdTable(XmlDocument^ doc)
{
	XmlNode^ mProduction = doc->SelectSingleNode("//m_Production");
	int count = Convert::ToInt32(mProduction->Attributes["Count"]->Value);

	vector<Production> prodTable(count);

	XmlNodeList^ prodNodeList = mProduction->ChildNodes;
	for (int i = 0; i <= prodNodeList->Count - 1; i++)
	{
		XmlNode^ prodNode = prodNodeList[i];
		int index = Convert::ToInt32(prodNode->Attributes["Index"]->Value);
		int nonTerminalIndex = Convert::ToInt32(prodNode->Attributes["NonTerminalIndex"]->Value);
		int symbolCount = Convert::ToInt32(prodNode->Attributes["SymbolCount"]->Value);

		XmlNodeList^ prodSymbolList = prodNode->ChildNodes;

		int left = nonTerminalIndex;
		vector<int> rightList = {};
		for (int j = 0; j <= prodSymbolList->Count - 1; j++)
		{
			XmlNode^ prodSymbol = prodSymbolList[j];
			int symbolIndex = Convert::ToInt32(prodSymbol->Attributes["SymbolIndex"]->Value);
			rightList.push_back(symbolIndex);
		}

		prodTable[index].left = left;
		prodTable[index].rightList = rightList;
	}

	return prodTable;
}

void printVarTable(map<string, double> varTable)
{
	map<std::string, double>::iterator it = varTable.begin();
	while (it != varTable.end())
	{
		string key = it->first;
		double value = it->second;
		cout << key << "  " << value << endl;
		it++;
	}
}

void printOutputList(vector<string> outputList)
{
	string token;
	for (unsigned int i = 0; i < outputList.size(); i++)
		cout << outputList[i];
}

int main()
{
	XmlDocument^ doc = gcnew XmlDocument();
	doc->Load("fullGrammar.xml");

	// Get input
	string filename = "";
	string inPath = "";
	while (true)
	{
		cout << "Please enter filename: ";
		cin >> filename;
		inPath = "Input/" + filename;
		if (fileExists(inPath) == false)
			cout << "Cannot find " + inPath + ". Please check filepath. " << endl;
		else
			break;
	}

	vector<CharSet> charTable = buildCharTable(doc);
	vector<Symbol> symbolTable = buildSymbolTable(doc);
	DFATable dt = buildDFATable(doc);

	TokenData result = tokenizeFile(charTable, symbolTable, dt, inPath);

	vector<string> tokenList = result.tokenList;
	vector<int> symbolList = result.symbolList;

	vector<vector<Cell>> parseTable = buildLALRTable(doc,symbolTable.size());
	vector<Production> prodTable = buildProdTable(doc);

	stack<int> parseStack;
	stack<string> semanticStack;
	map<string, double> varTable;
	vector<string> outputList;

	int currState; string varName;
	string currToken; string symbolType;
	int currSymbol; int action; int value; int lhsValue;
	double answer; bool acceptFlag = false;
	bool firstVar = true;
	vector<int> rightList; unsigned int pos = 0;

	parseStack.push(0);
	while (pos <= tokenList.size() - 1)
	{
		currState = parseStack.top();
		currToken = tokenList[pos];
		currSymbol = symbolList[pos];
		symbolType = symbolTable[symbolList[pos]].name;
		action = parseTable[currState][currSymbol].action;
		value = parseTable[currState][currSymbol].value;

		if (symbolType == "VARIABLE")
		{
			if (firstVar == true)
			{
				varName = currToken;
				firstVar = false;
			}
			else
			{
				double newToken = varTable[currToken];
				currToken = to_string(newToken);
			}
		}

		switch (action)
		{
		case 1:
			if (currToken == ";")
			{
				while (!parseStack.empty()) { parseStack.pop(); }
				double rhsValue = stof(semanticStack.top());
				while (!semanticStack.empty()) { semanticStack.pop(); }
				varTable[varName] = rhsValue;
				varName = "";
				firstVar = true;
				parseStack.push(0);
				outputList.push_back("; => ");
				outputList.push_back(to_string(rhsValue));
				outputList.push_back("\n");
			}
			else
			{
				parseStack.push(currSymbol);
				parseStack.push(value);
				semanticStack.push(currToken);
				outputList.push_back(tokenList[pos]);
			}
			pos = pos + 1;
			break;
		case 2:
			rightList = prodTable[value].rightList;

			// Minus
			if (rightList.size() == 2)
			{
				string item2 = semanticStack.top(); semanticStack.pop();
				string item1 = semanticStack.top(); semanticStack.pop();
				if (item1 == "-")
					answer = -1 * stof(item2);

				semanticStack.push(to_string(answer));
			}

			// Add, Subtract, Multiply, Divide, Exponent, Parentheses
			if (rightList.size() == 3)
			{
				string item3 = semanticStack.top(); semanticStack.pop();
				string item2 = semanticStack.top(); semanticStack.pop();
				string item1 = semanticStack.top(); semanticStack.pop();

				if (item2 == "+")
					answer = stof(item1) + stof(item3);
				else if (item2 == "-")
					answer = stof(item1) - stof(item3);
				else if (item2 == "*")
					answer = stof(item1) * stof(item3);
				else if (item2 == "/")
					answer = stof(item1) / stof(item3);
				else if (item2 == "^")
					answer = (double)pow(stof(item1), stof(item3));
				else if (item1 == "(" && item3 == ")")
					answer = stof(item2);

				semanticStack.push(to_string(answer));
			}

			// MOD and DIV
			if (rightList.size() == 6)
			{
				string item6 = semanticStack.top(); semanticStack.pop();
				string item5 = semanticStack.top(); semanticStack.pop();
				string item4 = semanticStack.top(); semanticStack.pop();
				string item3 = semanticStack.top(); semanticStack.pop();
				string item2 = semanticStack.top(); semanticStack.pop();
				string item1 = semanticStack.top(); semanticStack.pop();
				int answer = 0;
				if (item1 == "MOD")
					answer = stoi(item2) % stoi(item4);
				else if (item1 == "DIV")
					answer = stoi(item2) / stoi(item4);

				semanticStack.push(to_string(answer));
			}

			for (unsigned j = 0; j <= rightList.size() - 1; j++)
			{
				parseStack.pop(); // pop state
				parseStack.pop(); // pop token
			}

			currState = parseStack.top();
			lhsValue = prodTable[value].left;
			parseStack.push(lhsValue);
			parseStack.push(parseTable[currState][lhsValue].value);

			break;
		case 3:
			break;
		case 4:
			acceptFlag = true;
			break;
		default:
			while (!parseStack.empty()) { parseStack.pop(); }
			while (!semanticStack.empty()) { semanticStack.pop(); }
			parseStack.push(0);
			varName = "";
			firstVar = true;

			while (tokenList[pos] != ";") 
			{
				outputList.push_back(tokenList[pos]);
				pos = pos + 1; 
			}
			outputList.push_back("; => ERRROR\n");
			pos = pos + 1;
		}

		if (acceptFlag == true) break;
	}

	// Screen Output
	cout << "Results" << endl;
	cout << "=============" << endl;
	printOutputList(outputList);

	cout << endl;
	cout << "Symbol Table" << endl;
	cout << "==============" << endl;
	printVarTable(varTable);

	// File Output
	ofstream outFile;
	outFile.open("Output.txt");

	outFile << "Results" << endl;
	outFile << "=============" << endl;
	for (unsigned int i = 0; i < outputList.size(); i++)
		outFile << outputList[i];

	outFile << endl;
	outFile << "Symbol Table" << endl;
	outFile << "==============" << endl;
	map<string, double>::iterator it = varTable.begin();
	while (it != varTable.end())
	{
		cout << setprecision(4);
		string key = it->first;
		double value = it->second;
		outFile << key << "  " << value << endl;
		it++;
	}

	outFile.close();

	system("pause");
	return 0;
}