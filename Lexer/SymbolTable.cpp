// Project 3

#ifndef Symbol_Table_H
#define Symbol_Table_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

const int TABLE_SIZE = 1000;

struct TableNode {
	string lexeme = "";
	string type = "";
	int address = 0;
	int value = 0; // may not be applicable for symbol table, we shall see.
};

class SymbolTable
{
public:
	// Constructor
	SymbolTable()
	{
		symbolTableList.open("Symbol_Table.txt", ios::out);
	}

	~SymbolTable()
	{
		symbolTableList.close();
	}

	bool existsInTable(string newIdentifier) {
		bool result = false;
		for (int i = 0; i < numOfElements; ++i) {
			if (symbolTableArray[i].lexeme == newIdentifier) {
				result = true;
			}
		}

		return result;
	}

	void insert(string newIdentifier, string newType) {
		if (!existsInTable(newIdentifier))
		{
			symbolTableArray[numOfElements].lexeme = newIdentifier;
			symbolTableArray[numOfElements].type = newType;
			symbolTableArray[numOfElements].address = currentAddress;
			++currentAddress;
			++numOfElements;
		}
		else
		{
			cerr << "FATAL ERROR! LEXEME ALREADY EXISTS IN TABLE: " << newIdentifier << endl << endl;
		}

	}

	int getAddress(string identifier)
	{
		int result = 0;

		for (int i = 0; i < numOfElements; ++i)
		{
			if (symbolTableArray[i].lexeme == identifier)
			{
				result = symbolTableArray[i].address;
				i = numOfElements;
			}
		}

		return result;
	}

	string getType(string identifier)
	{
		string result;

		for (int i = 0; i < numOfElements; ++i)
		{
			if (symbolTableArray[i].lexeme == identifier)
			{
				result = symbolTableArray[i].type;
				i = numOfElements;
			}
		}

		return result;
	}

	void print() {

		symbolTableList << "Lexeme\tAddress\tType" << endl;

		for (int i = 0; i < numOfElements; ++i) {
			symbolTableList << symbolTableArray[i].lexeme << " ";
			symbolTableList << symbolTableArray[i].address << " ";
			symbolTableList << symbolTableArray[i].type;
			symbolTableList << endl;
		}
	}

private:

	int currentAddress = 2000;
	TableNode symbolTableArray[TABLE_SIZE];
	int numOfElements = 0;
	fstream symbolTableList;

};

#endif // Symbol_Table_H